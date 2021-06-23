
#include <SocketImpl.h>
#include <commproto/logger/Logging.h>	
#include <commproto/service/Connection.h>
#include <commproto/config/ConfigParser.h>
#include <commproto/logger/FileLogger.h>
#include <commproto/device/AuthChains.h>
#include <chrono>
#include <commproto/service/ServiceChains.h>
#include <commproto/control/endpoint/UIController.h>
#include <commproto/control/endpoint/UIFactory.h>
#include <commproto/parser/ParserDelegatorUtils.h>
#include <commproto/control/endpoint/ParserDelegatorUtils.h>
#include <commproto/endpoint/DelegatorProvider.h>
#include <commproto/endpoint/ChannelParserDelegator.h>
#include <commproto/endpoint/ParserDelegatorFactory.h>
#include <commproto/service/DiagnosticChains.h>
#include <plant/interface/PlantMessages.h>
#include "Numbers.h"
#include "PercentageSingleHealthTracker.h"
#include "PlantHandlers.h"
#include "ConfigValues.h"
#include "PlantHealthProvider.h"

using namespace commproto;


int main(int argc, const char * argv[]) {

	//testNumbers();

	const char * configFile;
	if (argc <= 1)
	{
		configFile = "plant.cfg";
	}
	else
	{
		configFile = argv[1];
	}

	rapidjson::Document doc = config::ConfigParser(configFile).get();

	bool logToConsole = config::getValueOrDefault(doc, ConfigValues::logToConsole, ConfigValues::logToConsoleDefault);


	logger::FileLogger logger("plant_health_log_" + logger::FileLogger::getTimestamp() + ".txt");
	if (!logToConsole)
	{
		logger.open();
		logger::setLoggable(&logger);
	}

	const int32_t port = config::getValueOrDefault(doc, ConfigValues::serverPort, ConfigValues::defaultServerPort);
	const char * const address = config::getValueOrDefault(doc, ConfigValues::serverAddress, ConfigValues::serverAddressDefault);

	const char * const epName = config::getValueOrDefault(doc, ConfigValues::targetEpName, ConfigValues::targetEpNameDefault);

	LOG_INFO("Plant Health service attempting to connect to %s:%d", address, port);


	SenderMapping::InitializeName("Service::PlantHealth");
	sockets::SocketHandle socket = std::make_shared<sockets::SocketImpl>();
	if (!socket->initClient(address, port))
	{
		LOG_ERROR("A problem occurred while starting service, shutting down...");
		return 1;
	}

	LOG_INFO("Plant Health started...");

	//send ptr size
	socket->sendByte(sizeof(void*));

	//core dependencies
	messages::TypeMapperHandle mapper = messages::TypeMapperFactory::build(socket);

	//registering our channel name
	uint32_t registerId = mapper->registerType<service::RegisterChannelMessage>();
	service::RegisterChannelMessage nameMsg(registerId, SenderMapping::getName());

	Message nameSerialized = service::RegisterChannelSerializer::serialize(std::move(nameMsg));
	socket->sendBytes(nameSerialized);

	//requesting connected channels
	uint32_t reqChannelID = mapper->registerType<diagnostics::RequestAllConnections>();


	// ui definition
	auto uiFactory = std::make_shared<control::endpoint::UIFactory>("myUI", mapper, socket);
	control::endpoint::UIControllerHandle controller = uiFactory->makeController();

	PercentageSensorTracker soilTracker(AbsoluteToPercentage(Interval<uint32_t>(0, 4096)), Interval<float>(40, 60), Interval<float>(0, 100));
	PercentageSingleHealthTracker soilHealthTracker(*uiFactory.get(), "Soil Humidity", soilTracker, Interval<uint32_t>(1200, 3500));


	PercentageSensorTracker uvTracker(AbsoluteToPercentage(Interval<uint32_t>(0, 4096)), Interval<float>(60, 100), Interval<float>(0, 100));
	PercentageSingleHealthTracker uvHealthTracker(*uiFactory.get(), "UV Exposure", uvTracker, Interval<uint32_t>(0,200));

	auto inputHelper = std::make_shared<InputHelper>(soilHealthTracker,uvHealthTracker);


	std::shared_ptr<PlantHealthProvider> provider = std::make_shared<PlantHealthProvider>(mapper, controller, inputHelper);
	endpoint::ChannelParserDelegatorHandle channelDelegator = std::make_shared<endpoint::ChannelParserDelegator>(provider);
	parser::ParserDelegatorHandle delegator = endpoint::ParserDelegatorFactory::build(channelDelegator);
	channelDelegator->addDelegator(0, delegator);

	parser::MessageBuilderHandle builder = std::make_shared<parser::MessageBuilder>(socket, channelDelegator);


	uint32_t subscribeMsgId = mapper->registerType<service::SubscribeMessage>();


	auto sub = service::SubscribeSerializer::serialize(service::SubscribeMessage(subscribeMsgId, epName));



	LOG_INFO("Waiting to acquire ID");
	//wait until we are sure we have an ID
	do
	{
		builder->pollAndRead();
	} while (SenderMapping::getId() == 0);
	LOG_INFO("Acquired ID: %d", SenderMapping::getId());

	socket->sendBytes(sub);

	while (true)
	{
		builder->pollAndReadTimes(100);
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
	socket->shutdown();
	return 0;
}
