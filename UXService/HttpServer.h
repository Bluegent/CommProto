#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H
#include <Poco/Net/HTTPRequestHandlerFactory.h>
#include <commproto/control/ux/UxControllers.h>
#include <Poco/Util/ServerApplication.h>
#include <Poco/Net/HTTPServer.h>
#include "HttpRequestHandler.h"
#include "LoginHandler.h"

class RequestHandlerFactory : public Poco::Net::HTTPRequestHandlerFactory
{
public:
	RequestHandlerFactory(const commproto::control::ux::UxControllersHandle& controller);
	Poco::Net::HTTPRequestHandler* createRequestHandler(const Poco::Net::HTTPServerRequest&) override;
	~RequestHandlerFactory();
private:
	void loadMimeMapping();
	void loadDB();
	commproto::control::ux::UxControllersHandle controller;
	KVMap mimeTypes;
	LoginHandlerHandle handler;
};

class UxServerApp : public Poco::Util::ServerApplication
{
public:
	UxServerApp(const commproto::control::ux::UxControllersHandle& controller, const uint32_t port);
protected:
	int main(const std::vector<std::string>&) override;
private:
	commproto::control::ux::UxControllersHandle controller;
	const uint32_t port;
};


#endif // HTTP_SERVER_H