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
		if (handler->hasSetup())
		{
			uri = "/login.html";
		}
		else
		{
			uri = "/setup.html";
		}
	}

	Poco::Path path(uri);
	std::string extension = Poco::toLower(path.getExtension());

	auto type = mimeTypes.find(extension);
	if (type == mimeTypes.end())
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

void UxRequestHandler::badRequest(Poco::Net::HTTPServerResponse& resp)
{
	resp.setStatus(Poco::Net::HTTPResponse::HTTP_BAD_REQUEST);
	resp.send().flush();
}

KVMap UxRequestHandler::parseRequest(Poco::Net::HTTPServerRequest& req)
{
	Poco::Net::HTMLForm form(req, req.stream());
	KVMap map;
	for (Poco::Net::NameValueCollection::ConstIterator i = form.begin(); i != form.end(); ++i)
	{
		map.emplace(i->first, i->second);
	};
	return map;
}


ActionData UxRequestHandler::parseBase(const KVMap& map) const
{

	ActionData result{};
	auto conn = map.find("connection");
	if (conn != map.end())
	{
		result.connection = conn->second;
	}
	auto tracker = map.find("session");
	if (tracker != map.end())
	{
		result.tracker = tracker->second;
	}
	auto id = map.find("controlId");
	if (id != map.end())
	{
		try
		{
			result.controlId = std::stoi(id->second);
		}
		catch (std::invalid_argument arg)
		{
			//do nothing
		}
	}
	return result;
}

void UxRequestHandler::handleButton(KVMap&& map) const
{
	ActionData data = parseBase(map);
	if (!data.controlId)
	{
		return;
	}

	auto controller = controllers->getController(data.connection);

	if (!controller)
	{
		return;
	}
	commproto::control::ux::ButtonHandle button = std::static_pointer_cast<commproto::control::ux::Button>(controller->getControl(data.controlId));

	if (!button)
	{
		return;
	}

	button->press();
}

void UxRequestHandler::handleSlider(KVMap&& map) const
{
	ActionData data = parseBase(map);
	if (!data.controlId)
	{
		return;
	}

	auto controller = controllers->getController(data.connection);
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
	commproto::control::ux::SliderHandle slider = std::static_pointer_cast<commproto::control::ux::Slider>(controller->getControl(data.controlId));

	if (!slider)
	{
		return;
	}
	controller->notifyTrackerUpdate(data.tracker, data.controlId);
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

UxRequestHandler::UxRequestHandler(const commproto::control::ux::UxControllersHandle& controllers, const KVMap & mimeTypes_, const LoginHandlerHandle & handler_)
	: controllers{ controllers }
	, mimeTypes(mimeTypes_)
	, handler{ handler_ }
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

std::string getWithoutCategory(const std::string & input)
{
	auto pos = input.find("::");
	if (pos == std::string::npos)
	{
		return input;
	}
	return input.substr(pos + 2);
}

void UxRequestHandler::handleUpdate(Poco::Net::HTTPServerRequest& req, Poco::Net::HTTPServerResponse& resp)
{
	std::string url = req.getURI();
	bool force = url.find("force") != std::string::npos;

	Poco::Net::HTMLForm form(req, req.stream());
	std::string tracker = form["session"];
	std::string selected = form["selected"];

	std::ostream& out = resp.send();

	resp.setStatusAndReason(Poco::Net::HTTPResponse::HTTP_OK);
	resp.setContentType("application/json");

	auto ctrls = controllers->getControllers();
	Poco::JSON::Object updateJson;
	//updates
	Poco::JSON::Object uiJSON;
	Poco::JSON::Array notifsJSON;
	Poco::JSON::Array controllerNames;
	Poco::JSON::Array removals;

	auto selectedController = controllers->getController(selected);
	if (selectedController) {
		if (force)
		{
			selectedController->addTracker(tracker);
		}
		//updates
		if (force || selectedController->hasUpdate(tracker))
		{
			auto updates = selectedController->getUpdates(tracker, force);

			uiJSON.set("name", selectedController->getConnectionName());
			Poco::JSON::Array cUpdates;
			for (auto controlUpdate : updates)
			{
				Poco::JSON::Object cUpdate;
				cUpdate.set("element", controlUpdate.first);
				cUpdate.set("controlString", controlUpdate.second);
				cUpdates.add(cUpdate);
			}
			uiJSON.set("updates", cUpdates);
		}
	}

	//notifications and sidebar updates from all controllers
	for (auto controller : ctrls) {

		Poco::JSON::Object ctrl;
		ctrl.set("name", controller.first);

		KVMap replacements;
		replacements.emplace("@sanitized", getWithoutCategory(controller.first));
		replacements.emplace("@name", controller.first);

		ctrl.set("control_string", controller.second->getEngine()->getTemplateWithReplacements("sidebar_item", std::move(replacements)));
		controllerNames.add(ctrl);

		if (force)
		{
			controller.second->addTracker(tracker);
		}

		auto removalUpdates = controller.second->getRemovals(tracker);
		for (auto removal : removalUpdates)
		{
			removals.add(removal);
		}

		//notifications
		if (force || controller.second->hasNotifications(tracker))
		{
			auto updates = controller.second->getNotifications(tracker, force);
			for (auto notifUpdate : updates)
			{
				Poco::JSON::Object cUpdate;
				cUpdate.set("name", notifUpdate.first);
				cUpdate.set("notification", notifUpdate.second);
				notifsJSON.add(cUpdate);
			}
		}
	}
	if (uiJSON.size() != 0)
	{
		updateJson.set("controller", uiJSON);
	}
	if (notifsJSON.size() != 0)
	{
		updateJson.set("notifications", notifsJSON);
	}

	if (removals.size() != 0)
	{
		updateJson.set("removals", removals);
	}
	updateJson.set("controllers", controllerNames);

	std::stringstream uis;
	updateJson.stringify(uis);
	out << uis.str();
	out.flush();
}

void UxRequestHandler::handleNotification(KVMap&& map) const
{
	ActionData data = parseBase(map);
	if (!data.controlId)
	{
		return;
	}
	auto controller = controllers->getController(data.connection);
	if (!controller)
	{
		return;
	}

	auto it = map.find("option");
	if (it == map.end())
	{
		return;
	}

	auto it2 = map.find("actionId");

	if (it2 == map.end())
	{
		return;
	}

	uint32_t actionId = -1;

	try
	{
		actionId = std::stoi(it2->second);
	}
	catch (std::invalid_argument arg)
	{
		return;
	}

	commproto::control::ux::NotificationHandle notif = std::static_pointer_cast<commproto::control::ux::Notification>(controller->getNotification(data.controlId));
	if (!notif)
	{
		return;
	}

	notif->execute(it->second, actionId);
	controller->dismissNotification(data.tracker, actionId);

}

void UxRequestHandler::handleToggle(KVMap&& map) const
{
	ActionData data = parseBase(map);
	if (!data.controlId)
	{
		return;
	}
	auto controller = controllers->getController(data.connection);
	if (!controller)
	{
		return;
	}
	commproto::control::ux::ToggleHandle toggle = std::static_pointer_cast<commproto::control::ux::Toggle>(controller->getControl(data.controlId));

	if (!toggle)
	{
		return;
	}
	controller->notifyTrackerUpdate(data.tracker, data.controlId);
	toggle->toggle();
}

void UxRequestHandler::handleAction(Poco::Net::HTTPServerRequest& req, Poco::Net::HTTPServerResponse& resp)
{
	KVMap map = parseRequest(req);
	resp.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
	resp.send().flush();
	parseKVMap(std::move(map));
}

void UxRequestHandler::handleLogin(Poco::Net::HTTPServerRequest& req, Poco::Net::HTTPServerResponse& resp)
{
	KVMap map = parseRequest(req);

	auto name = map.find("username");
	auto pwd = map.find("password");
	auto token = map.find("token");
	bool hasReq = name != map.end() && pwd != map.end() || token != map.end();

	if (!hasReq)
	{
		badRequest(resp);
		return;
		//how did you even get here?
	}

	if (token != map.end())
	{
		bool recognized = handler->validateToken(token->second);
		if (!recognized)
		{
			badRequest(resp);
			return;
		}


	}

	if (!handler->hasSetup())
	{
		badRequest(resp);
		return;
	}

	bool recognized = handler->validate(name->second, pwd->second);
	if (!recognized)
	{
		badRequest(resp);
		return;
	}

	std::string newtoken = handler->generateToken();
	handler->saveToken(newtoken);
	resp.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
	std::ostream& out = resp.send();
	out << newtoken;
	out.flush();
}

void UxRequestHandler::handleSetup(Poco::Net::HTTPServerRequest& req, Poco::Net::HTTPServerResponse& resp)
{
	KVMap map = parseRequest(req);
	auto name = map.find("username");
	auto pwd = map.find("password");
	auto serial = map.find("serial");
	bool hasReq = name != map.end() && pwd != map.end() || serial != map.end();

	if (!hasReq)
	{
		badRequest(resp);
		return;
		//how did you even get here?
	}


	if (handler->hasSetup())
	{
		badRequest(resp);
		return;
	}

	if (serial == map.end())
	{
		badRequest(resp);
		return;
	}

	bool goodSerial = handler->validateSerial(serial->second);
	if (!goodSerial)
	{
		badRequest(resp);
		return;
	}

	handler->saveUserDetails(name->second, pwd->second);
	resp.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
	std::ostream& out = resp.send();
	out.flush();
}

void UxRequestHandler::handlePost(Poco::Net::HTTPServerRequest& req, Poco::Net::HTTPServerResponse& resp)
{
	std::string url = req.getURI();
	if (url.find("/update") == 0)
	{
		handleUpdate(req, resp);
		return;
	}
	if (url.compare("/action") == 0)
	{
		handleAction(req, resp);
		return;
	}
	if (url.compare("/login") == 0)
	{
		handleLogin(req, resp);
		return;
	}
	if (url.compare("/setup") == 0)
	{
		handleSetup(req, resp);
		return;
	}

}
