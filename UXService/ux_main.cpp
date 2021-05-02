#include <commproto/service/Dispatch.h>
#include <SocketImpl.h>
#include <commproto/logger/Logging.h>
#include <commproto/messages/SenderMaping.h>
#include <commproto/service/ServiceChains.h>
#include <commproto/messages/TypeMapper.h>
#include <commproto/endpoint/ChannelParserDelegator.h>
#include <commproto/endpoint/ParserDelegatorFactory.h>
#include <commproto/control/ux/UxControllers.h>
#include <commproto/config/ConfigParser.h>
#include <commproto/logger/FileLogger.h>

#include "HttpServer.h"
#include <commproto/control/ux/UxDelegatorProvider.h>
#include <commproto/control/ux/TemplateEngine.h>
#include "TemplateEngineLoader.h"

namespace ConfigValues
{
	static constexpr const char * const serverAddr = "serverAddr";
	static constexpr const char * const defaultServerAddr = "localhost";

	static constexpr const char * const serverPort = "serverPort";
    static constexpr const int32_t defaultServerPort = 25565;

	static constexpr const char * const httpPort = "httpPort";
    static constexpr const int32_t defaultHttpPort = 9090;

	static constexpr const char * const channelName = "channelName";
	static constexpr const char * const defaultChannelName = "Service::UX";

	static constexpr const char * const logToConsole = "logToConsole";
	static constexpr const bool logToConsoleDefault = true;
};

using namespace commproto;

void websiteLoop(int argc, char * argv[], const uint32_t port, const commproto::control::ux::UxControllersHandle & controllers)
{
	UxServerApp app(controllers, port);
	app.run(argc, argv);
}

int main(int argc, char * argv[])
{ 
    const char * configFile;
    if(argc <= 1)
    {
        configFile = "uxservice.cfg";
    }
    else
    {
        configFile = argv[1];
    }

    rapidjson::Document doc = commproto::config::ConfigParser(configFile).get();

    const int32_t dispatchPort = config::getValueOrDefault(doc, ConfigValues::serverPort, ConfigValues::defaultServerPort);
	const char * const dispatchAddr = config::getValueOrDefault(doc, ConfigValues::serverAddr, ConfigValues::defaultServerAddr);
    const int32_t httpPort = config::getValueOrDefault(doc, ConfigValues::httpPort, ConfigValues::defaultHttpPort);
	const char * const name = config::getValueOrDefault(doc, ConfigValues::channelName, ConfigValues::defaultChannelName);
	bool logToConsole = config::getValueOrDefault(doc, ConfigValues::logToConsole, ConfigValues::logToConsoleDefault);

	logger::FileLogger logger("ux_log_" + logger::FileLogger::getTimestamp() + ".txt");
	if (!logToConsole)
	{
		logger.open();
		commproto::logger::setLoggable(&logger);
	}

	SenderMapping::InitializeName(name);

	auto engine = std::make_shared<control::ux::TemplateEngine>();
	TemplateEngineLoader loader(engine);
	Poco::Path dir("html_files");
	dir.append("templates");
	std::string path = dir.toString();
	loader.load(path);

	sockets::SocketHandle socket = std::make_shared<sockets::SocketImpl>();
	if (!socket->initClient(dispatchAddr, dispatchPort))
	{
		LOG_ERROR("A problem occurred while starting endpoint, shutting down...");
		return 0;
	}

	LOG_INFO("UX service started...");

	//send ptr size
	socket->sendByte(sizeof(void*));

	//core dependencies
	messages::TypeMapperHandle mapper = messages::TypeMapperFactory::build(socket);

	//registering our channel name
	uint32_t registerId = mapper->registerType<service::RegisterChannelMessage>();
	service::RegisterChannelMessage nameMsg(registerId, SenderMapping::getName());
	Message nameSerialized = service::RegisterChannelSerializer::serialize(std::move(nameMsg));
	socket->sendBytes(nameSerialized);

	//delegator to parse incoming messages
	control::ux::UxControllersHandle controllers = std::make_shared<control::ux::UxControllers>();
	std::shared_ptr<control::ux::UXServiceProvider> provider = std::make_shared<control::ux::UXServiceProvider>(mapper, socket, controllers,engine);
	endpoint::ChannelParserDelegatorHandle channelDelegator = std::make_shared<endpoint::ChannelParserDelegator>(provider);

	endpoint::MappingNotification removeController = [controllers](const std::string & name, const uint32_t id)
	{
		LOG_INFO("Removing UX controller for connection %s(%d)", name.c_str(), id);
		controllers->removeController(name);
	};
	channelDelegator->subscribeToChannelRemoval(removeController);
	parser::ParserDelegatorHandle delegator = endpoint::ParserDelegatorFactory::build(channelDelegator);
	channelDelegator->addDelegator(0, delegator);

	//subscribe - unsubscribe
	uint32_t registerSubId = mapper->registerType<service::SubscribeMessage>();
	uint32_t unsubId = mapper->registerType<service::UnsubscribeMessage >();
	uint32_t sendtoId = mapper->registerType<service::SendToMessage>();

	parser::MessageBuilderHandle builder = std::make_shared<parser::MessageBuilder>(socket, channelDelegator);


    LOG_INFO("Waiting to acquire ID");

	//wait until we are sure we have an ID
	do
	{
		builder->pollAndRead();
	} while (SenderMapping::getId() == 0);
    LOG_INFO("Acquired ID: %d",SenderMapping::getId());

	service::SubscribeMessage sub(registerSubId, "");
	socket->sendBytes(service::SubscribeSerializer::serialize(std::move(sub)));

	std::thread websiteThread(websiteLoop, argc, argv, httpPort, controllers);

	while (true) {
		builder->pollAndReadTimes(100u);
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
	return 0;
}
