#ifndef HTTP_REQUEST_HANDLER_H
#define HTTP_REQUEST_HANDLER_H
#include <map>
#include <Poco/Net/HTTPRequestHandler.h>
#include <commproto/control/ux/UxControllers.h>
#include <Poco/Net/HTTPResponse.h>


enum class ControlType : uint8_t
{
	Button = 0,
	Slider,
	Toggle,
	Label,
	Notification
};

using KVMap = std::map<std::string, std::string>;

class UxRequestHandler : public Poco::Net::HTTPRequestHandler
{
public:
	UxRequestHandler(const commproto::control::ux::UxControllersHandle& controllers, const KVMap & mimeTypes_);
	void handleRequest(Poco::Net::HTTPServerRequest& req, Poco::Net::HTTPServerResponse& resp) override;

private:
	void handleUpdate(Poco::Net::HTTPServerRequest& req, Poco::Net::HTTPServerResponse& resp);
	void handleAction(Poco::Net::HTTPServerRequest& req, Poco::Net::HTTPServerResponse& resp);

	void handlePost(Poco::Net::HTTPServerRequest& req, Poco::Net::HTTPServerResponse& resp);
	void handleGet(Poco::Net::HTTPServerRequest& req, Poco::Net::HTTPServerResponse& resp);

	void handleBase(const KVMap& map, std::string& connection, uint32_t& controlId) const;
	void handleButton(KVMap&& map) const;
	void handleToggle(KVMap&& map) const;
	void handleNotification(KVMap && map) const;
	void handleSlider(KVMap && map) const;
	void parseKVMap(KVMap&& map) const;
	commproto::control::ux::UxControllersHandle controllers;
	const KVMap& mimeTypes;

};


#endif// HTTP_REQUEST_HANDLER_H