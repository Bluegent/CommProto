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
#include <commproto/control/ux/HtmlUxContainer.h>
#include <sstream>

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

bool UxRequestHandler::hasValidToken(const KVMap& map)
{
	auto it = map.find("token");
	if(it == map.end())
	{
		return false;
	}

	return handler->validateToken(it->second);
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

	return result;
}

void UxRequestHandler::parseKVMap(KVMap&& map) const
{
	auto type = map.find("controlType");
	if (type == map.end())
	{
		return;
	}
	ActionData data = parseBase(map);

	auto controller =  controllers->getController(data.connection);
	if(!controller)
	{
		return;
	}
	controller->handle(std::move(map));
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

void UxRequestHandler::handleUpdate(Poco::Net::HTTPServerRequest& req, Poco::Net::HTTPServerResponse& resp, KVMap & map)
{
	std::string url = req.getURI();
	bool force = url.find("force") != std::string::npos;

	std::string tracker = map["session"];
	std::string selected = map["selected"];


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
				std::string updateString(std::static_pointer_cast<commproto::control::ux::HtmlUxContainer>(controlUpdate.second)->html);
				cUpdate.set("controlString", updateString);
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
				std::string updateString(std::static_pointer_cast<commproto::control::ux::HtmlUxContainer>(notifUpdate.second)->html);
				cUpdate.set("notification", updateString);
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


void UxRequestHandler::handleAction(Poco::Net::HTTPServerRequest& req, Poco::Net::HTTPServerResponse& resp, KVMap & map)
{
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
		resp.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
		resp.send().flush();
		return;

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
	auto remember = map.find("remember");
	bool longterm = false;
	if(remember != map.end())
	{
		longterm = remember->second == "t";
	}
	std::string newtoken = handler->generateToken();
	handler->saveToken(newtoken,longterm);
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

	KVMap map = parseRequest(req);

	if (url.find("/check_token") ==0)
	{
		if (!hasValidToken(map))
		{
			badRequest(resp);
			return;
		}
		resp.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
		resp.send().flush();
		return;
	}


	if(!hasValidToken(map))
	{
		badRequest(resp);
		return;
	}


	if (url.find("/update") == 0)
	{
		handleUpdate(req, resp,map);
		return;
	}
	if (url.compare("/action") == 0)
	{
		handleAction(req, resp,map);
		return;
	}


}
