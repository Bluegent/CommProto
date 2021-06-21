
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

using namespace commproto;

namespace ConfigValues
{
	static constexpr const char * const logToConsole = "logToConsole";
	static constexpr const bool logToConsoleDefault = true;

	static constexpr const char * const serverPort = "serverPort";
	static constexpr const int32_t defaultServerPort = 25565;

	static constexpr const char * const serverAddress = "serverAddress";
	static constexpr const char * const serverAddressDefault = "127.0.0.1";

	static constexpr const char * const targetEpName = "targetEpName";
	static constexpr const char * const targetEpNameDefault = "Endpoint::SmartPot";

};


class PlantHealthTracker
{
public:
	void setSoilCalibration(const uint32_t dry, const uint32_t wet);
	void setUvCalibration(const uint32_t dark, const uint32_t light);
	void setSoilReading(const uint32_t value);
	void setUvReading(const uint32_t value);
private:
	SensorTracker<uint32_t> soil;
	SensorTracker<uint32_t> uv;
};

class InputHelper
{
	public:
		void notifyTempHum(const float temp, const float hum);
		void notifySoil(const uint32_t soil);
		void notifyUV(const uint32_t uv);
	private:
		control::endpoint::UIControllerHandle controller;
};


parser::ParserDelegatorHandle buildSelfDelegator()
{
	std::shared_ptr<parser::ParserDelegator> delegator = std::make_shared<parser::ParserDelegator>();
	parser::DelegatorUtils::buildBase(delegator);
	return delegator;
}


class PlantHealthProvider : public endpoint::DelegatorProvider {
public:
	PlantHealthProvider(const messages::TypeMapperHandle & mapper_, const control::endpoint::UIControllerHandle & controller_)
		: mapper{ mapper_ }
		, controller{ controller_ }
	{

	}
	parser::ParserDelegatorHandle provide(const std::string& name, const uint32_t id) override
	{
		parser::ParserDelegatorHandle delegator = buildSelfDelegator();
		control::endpoint::DelegatorUtils::addParsers(delegator, controller);

		return delegator;
	}
private:
	messages::TypeMapperHandle mapper;
	control::endpoint::UIControllerHandle controller;
};



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


	std::shared_ptr<PlantHealthProvider> provider = std::make_shared<PlantHealthProvider>(mapper, controller);
	endpoint::ChannelParserDelegatorHandle channelDelegator = std::make_shared<endpoint::ChannelParserDelegator>(provider);
	parser::ParserDelegatorHandle delegator = endpoint::ParserDelegatorFactory::build(channelDelegator);
	channelDelegator->addDelegator(0, delegator);

	parser::MessageBuilderHandle builder = std::make_shared<parser::MessageBuilder>(socket, channelDelegator);

	LOG_INFO("Waiting to acquire ID");
	//wait until we are sure we have an ID
	do
	{
		builder->pollAndRead();
	} while (SenderMapping::getId() == 0);
	LOG_INFO("Acquired ID: %d", SenderMapping::getId());


	while (true)
	{
		builder->pollAndReadTimes(100);
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
	socket->shutdown();
	return 0;
}
