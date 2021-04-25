#include "HttpServer.h"
#include <commproto/logger/Logging.h>
#include <Poco/JSON/Parser.h>
#include "FileLoader.h"

RequestHandlerFactory::RequestHandlerFactory(const commproto::control::ux::UxControllersHandle& controller)\
	: controller{ controller }
{
	loadMimeMapping();
	loadDB();
}

Poco::Net::HTTPRequestHandler* RequestHandlerFactory::createRequestHandler(const Poco::Net::HTTPServerRequest&)
{
	return new UxRequestHandler(controller, mimeTypes, handler);
}

RequestHandlerFactory::~RequestHandlerFactory()
{
	if(handler)
	{		
		handler->save();
	}
}


void RequestHandlerFactory::loadMimeMapping()
{
	LOG_INFO("Loading MIME type mappings");


	Poco::Path path("html_files");
	path.append("config").append("suppported_files.json");
	std::string contents = FileLoader::getFileContents(path.absolute().toString());;

	Poco::JSON::Parser parser;
	Poco::Dynamic::Var result = parser.parse(contents);
	Poco::JSON::Object::Ptr root = result.extract<Poco::JSON::Object::Ptr>();
	for (auto it = root->begin(); it != root->end(); ++it)
	{
		mimeTypes.emplace(it->first, it->second);
	}

	LOG_INFO("Loaded %d MIME type mappings", mimeTypes.size());

}

void RequestHandlerFactory::loadDB()
{
	Poco::Path path("html_files");
	path.append("config").append("db.bin");

	handler = std::make_shared<JSONLoginHandler>(path.toString());
	if (!handler->load())
	{
		LOG_WARNING("Database file could not be located, perhaps this is the initial setup");
		return;
	}
	if(handler->validate("user", "password"))
	{
		LOG_INFO("Load good!");
	}
}

UxServerApp::UxServerApp(const commproto::control::ux::UxControllersHandle& controller, const uint32_t port): controller{controller}, port{port}
{

}

int UxServerApp::main(const std::vector<std::string>&)
{
	Poco::Net::HTTPServer server(new RequestHandlerFactory(controller), Poco::Net::ServerSocket(port), new Poco::Net::HTTPServerParams);

	server.start();
	LOG_INFO("HTTP Server started");

	waitForTerminationRequest(); // wait for CTRL-C or kill

	LOG_INFO("HTTP Server shutting down");
	server.stop();

	return Application::EXIT_OK;
}
