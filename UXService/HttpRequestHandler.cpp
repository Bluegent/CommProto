#include "HttpRequestHandler.h"
#include <commproto/control/ux/Button.h>
#include <commproto/logger/Logging.h>
#include <Poco/Net/HTMLForm.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>
#include <fstream>
#include <commproto/control/ux/Toggle.h>
#include <commproto/control/ux/Slider.h>
#include <Poco/Path.h>
#include <Poco/JSON/Array.h>
#include <Poco/JSON/Object.h>
#include <Poco/StreamCopier.h>
#include <sstream>

const std::map<std::string, ControlType> stringMap = {
	{ "button",		ControlType::Button },
	{ "slider",		ControlType::Slider },
	{ "toggle",		ControlType::Toggle },
	{ "label",		ControlType::Label },
	{ "slider",		ControlType::Slider },
	{"notification",ControlType::Notification}
};

void UxRequestHandler::handleGet(Poco::Net::HTTPServerRequest& req, Poco::Net::HTTPServerResponse& resp)
{
	std::string uri = req.getURI();
	if (uri == "/")
	{
		uri = "/index.html";
	}

	Poco::Path path(uri);
	std::string extension = Poco::toLower(path.getExtension());

	auto type = mimeTypes.find(extension);
	if(type == mimeTypes.end())
	{
		resp.setStatus(Poco::Net::HTTPResponse::HTTP_BAD_REQUEST);
		return;
	}
	resp.setContentType(type->second);
	resp.setStatus(Poco::Net::HTTPResponse::HTTP_OK);

	Poco::Path root("html_files");
	std::string reqFile = uri.substr(1);
	root.append(reqFile);
	std::ostream& out = resp.send();
	std::ifstream file(root.toString(), std::ifstream::in | std::ifstream::binary);
	if (file.is_open())
	{
		Poco::StreamCopier::copyStream(file, out);
	}
	file.close();
	out.flush();
}

void UxRequestHandler::handleBase(const KVMap& map, std::string& connection, uint32_t& controlId) const
{
	auto conn = map.find("connection");
	if (conn != map.end())
	{
		connection = conn->second;
	}
	auto id = map.find("controlId");
	if (id != map.end())
	{
		try
		{
			controlId = std::stoi(id->second);
		}
		catch (std::invalid_argument arg)
		{
			//do nothing
		}
	}
}

void UxRequestHandler::handleButton(KVMap&& map) const
{
	std::string connection = "";
	uint32_t controlId = 0;
	handleBase(map, connection, controlId);
	auto controller = controllers->getController(connection);
	if (!controller)
	{
		return;
	}
	commproto::control::ux::ButtonHandle button = std::static_pointer_cast<commproto::control::ux::Button>(controller->getControl(controlId));

	if (!button)
	{
		return;
	}

	button->press();
}

void UxRequestHandler::handleSlider(KVMap&& map) const
{
	std::string connection = "";
	uint32_t controlId = 0;
	handleBase(map, connection, controlId);
	auto controller = controllers->getController(connection);
	if (!controller)
	{
		return;
	}
	auto it = map.find("value");
	if (it == map.end())
	{
		return;
	}

	float value = 0.f;
	try
	{
		value = std::stof(it->second);
	}
	catch (std::invalid_argument arg)
	{
		return;
	}
	commproto::control::ux::SliderHandle slider = std::static_pointer_cast<commproto::control::ux::Slider>(controller->getControl(controlId));

	if (!slider)
	{
		return;
	}

	slider->setValue(value);

}


void UxRequestHandler::parseKVMap(KVMap&& map) const
{
	auto type = map.find("controlType");
	if (type == map.end())
	{
		return;
	}
	auto typeValue = stringMap.find(type->second);
	if (typeValue == stringMap.end())
	{
		return;
	}

	switch (typeValue->second)
	{
	case ControlType::Button:
		handleButton(std::move(map));
		break;
	case ControlType::Slider:
		handleSlider(std::move(map));
		break;
	case ControlType::Toggle:
		handleToggle(std::move(map));
		break;
	case ControlType::Label: break;
	case ControlType::Notification:
		handleNotification(std::move(map));
		break;
	default:;
	}
}

UxRequestHandler::UxRequestHandler(const commproto::control::ux::UxControllersHandle& controllers,const KVMap & mimeTypes_)
	: controllers{controllers}
	, mimeTypes(mimeTypes_)
{
}

void UxRequestHandler::handleRequest(Poco::Net::HTTPServerRequest& req, Poco::Net::HTTPServerResponse& resp)
{
	if (req.getMethod() == "POST")
	{
		handlePost(req, resp);
		return;
	}
	if (req.getMethod() == "GET")
	{
		handleGet(req, resp);
	}
}

void UxRequestHandler::handleUpdate(Poco::Net::HTTPServerRequest& req, Poco::Net::HTTPServerResponse& resp)
{
	std::string url = req.getURI();
	bool force = url.find("force") != std::string::npos;

	Poco::Net::HTMLForm form(req, req.stream());
	std::string tracker = form["session"];
	std::ostream& out = resp.send();

	resp.setStatusAndReason(Poco::Net::HTTPResponse::HTTP_OK);
	resp.setContentType("application/json");

	auto ctrls = controllers->getControllers();

	Poco::JSON::Array uisJSON;
	for (auto controller : ctrls)
	{
		if (force)
		{
			controller.second->addTracker(tracker);
		}
		if (force || controller.second->hasUpdate(tracker))
		{
			auto updates = controller.second->getUpdates(tracker, force);
			Poco::JSON::Object ui;
			ui.set("name", controller.second->getConnectionName());
			Poco::JSON::Array cUpdates;
			for (auto controlUpdate : updates)
			{
				Poco::JSON::Object cUpdate;
				cUpdate.set("element", controlUpdate.first);
				cUpdate.set("controlString", controlUpdate.second);
				cUpdates.add(cUpdate);
			}

			ui.set("updates", cUpdates);
			uisJSON.add(ui);
		}
	}

	if (uisJSON.size() != 0) {
		std::stringstream uis;
		uisJSON.stringify(uis);
		out << uis.str();
	}
	else
	{

		out << "<null>";
	}

	resp.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
	out.flush();
}

void UxRequestHandler::handleNotificationUpdate(Poco::Net::HTTPServerRequest& req, Poco::Net::HTTPServerResponse& resp)
{
	std::string url = req.getURI();
	bool update = controllers->hasNotifications() || (url.find("force") != std::string::npos);
	if (!update)
	{
		resp.setStatusAndReason(Poco::Net::HTTPResponse::HTTP_OK);
		resp.send() << "<null>";
		return;
	}
	auto ctrls = controllers->getControllers();
	resp.setStatusAndReason(Poco::Net::HTTPResponse::HTTP_OK);
	std::ostream& out = resp.send();
	resp.setContentType("text/html");

	std::stringstream stream;

	for (auto it = ctrls.begin(); it != ctrls.end(); ++it)
	{
		if (update || it->second->hasNotifications())
		{
			stream << it->second->getNotifications();
		}
	}
	out << stream.str();
}

void UxRequestHandler::handleNotification(KVMap&& map) const
{
	std::string connection = "";
	uint32_t controlId = 0;
	handleBase(map, connection, controlId);
	auto controller = controllers->getController(connection);
	if (!controller)
	{
		return;
	}

	auto it = map.find("option");
	if (it == map.end())
	{
		return;
	}

	commproto::control::ux::NotificationHandle notif = std::static_pointer_cast<commproto::control::ux::Notification>(controller->getNotification(controlId));
	if (!notif)
	{
		return;
	}

	notif->execute(it->second);
	controller->dismissNotification(controlId);

}

void UxRequestHandler::handleToggle(KVMap&& map) const
{
	std::string connection = "";
	uint32_t controlId = 0;
	handleBase(map, connection, controlId);
	auto controller = controllers->getController(connection);
	if (!controller)
	{
		return;
	}
	commproto::control::ux::ToggleHandle toggle = std::static_pointer_cast<commproto::control::ux::Toggle>(controller->getControl(controlId));

	if (!toggle)
	{
		return;
	}

	toggle->toggle();
}

void UxRequestHandler::handleAction(Poco::Net::HTTPServerRequest& req, Poco::Net::HTTPServerResponse& resp)
{
	std::string connection;
	std::string control;
	Poco::Net::HTMLForm form(req, req.stream());
	KVMap map;
	for (Poco::Net::NameValueCollection::ConstIterator i = form.begin(); i != form.end(); ++i)
	{
		map.emplace(i->first, i->second);
	}
	resp.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
	resp.send().flush();
	parseKVMap(std::move(map));
}

void UxRequestHandler::handlePost(Poco::Net::HTTPServerRequest& req, Poco::Net::HTTPServerResponse& resp)
{
	std::string url = req.getURI();
	if (url.find("/notification") == 0)
	{
		handleNotificationUpdate(req, resp);
		return;
	}
	if (url.find("/update") == 0)
	{
		handleUpdate(req, resp);
		return;
	}
	if (url.compare("/action") == 0)
	{
		handleAction(req,resp);
	}
}
