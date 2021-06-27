
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
#include "SettingHelper.h" 

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
	const char * const cfgFile = config::getValueOrDefault(doc, ConfigValues::cfgFileName, ConfigValues::cfgFileNameDefault);

	SettingsHelperHandle settingsHelper = std::make_shared<SettingsHelper>(cfgFile);
	bool load = settingsHelper->load();
	if(!load)
	{
		settingsHelper->save();
	}

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


	auto outputHelper = std::make_shared<OutputHelper>(socket, epName);

	uint32_t startIrrigationId = mapper->registerType<plant::TogglePump>();
	uint32_t startUvId = mapper->registerType<plant::ToggleUVLamp>();
	uint32_t sendToId = mapper->registerType <service::SendToMessage>();

	outputHelper->setIds(startIrrigationId, startUvId, sendToId);

	// ui definition
	auto uiFactory = std::make_shared<control::endpoint::UIFactory>("myUI", mapper, socket);
	control::endpoint::UIControllerHandle controller = uiFactory->makeController();

	ISettingUpdate soilCalibrateMin = [&settingsHelper](const uint32_t value)
	{
		settingsHelper->data.soil.calibrateMin = value;
		settingsHelper->save();
	};

	ISettingUpdate soilCalibrateMax = [&settingsHelper](const uint32_t value)
	{
		settingsHelper->data.soil.calibrateMax = value;
		settingsHelper->save();
	};

	ISettingUpdate uvCalibrateMin = [&settingsHelper](const uint32_t value)
	{
		settingsHelper->data.uv.calibrateMin = value;
		settingsHelper->save();
	};

	ISettingUpdate uvCalibrateMax = [&settingsHelper](const uint32_t value)
	{
		settingsHelper->data.uv.calibrateMax = value;
		settingsHelper->save();
	};

	FSettingUpdate soilDesireMin = [&settingsHelper](const float value)
	{
		settingsHelper->data.soil.desireMin = value;
		settingsHelper->save();
	};

	FSettingUpdate soilDesireMax = [&settingsHelper](const float value)
	{
		settingsHelper->data.soil.desireMax = value;
		settingsHelper->save();
	};

	FSettingUpdate uvDesireMin = [&settingsHelper](const float value)
	{
		settingsHelper->data.uv.desireMin = value;
		settingsHelper->save();
	};

	FSettingUpdate uvDesireMax = [&settingsHelper](const float value)
	{
		settingsHelper->data.uv.desireMax = value;
		settingsHelper->save();
	};

	Interval<float> soilDesire(settingsHelper->data.soil.desireMin, settingsHelper->data.soil.desireMax);
	Interval<uint32_t> soilCalibrate(settingsHelper->data.soil.calibrateMin, settingsHelper->data.soil.calibrateMax);
	PercentageSensorTracker soilTracker(AbsoluteToPercentage(Interval<uint32_t>(0, 4096)), soilDesire, Interval<float>(0, 100));
	SinglePTrackerHandle soilHealthTracker = std::make_shared<PercentageSingleHealthTracker>(*uiFactory.get(), "Soil Humidity", soilTracker, soilCalibrate,"Irrigation");
	soilHealthTracker->setUpdates(soilCalibrateMin, soilCalibrateMax, soilDesireMin, soilDesireMax);


	HealthTrackerAction onLower = [&outputHelper]()
	{
		LOG_INFO("Turning on irrigation...");
		outputHelper->startPump();
	};

	HealthTrackerAction onDesired = [&outputHelper]()
	{
		LOG_INFO("Turning off irrigation...");
		outputHelper->stopPump();
	};

	soilHealthTracker->setOnLower(onLower);
	soilHealthTracker->setOnDesired(onDesired);

	Interval<float> uvDesire(settingsHelper->data.uv.desireMin, settingsHelper->data.uv.desireMax);
	Interval<uint32_t> uvCalibrate(settingsHelper->data.uv.calibrateMin, settingsHelper->data.uv.calibrateMax);
	PercentageSensorTracker uvTracker(AbsoluteToPercentage(Interval<uint32_t>(0, 4096)), uvDesire, Interval<float>(0, 100));
	SinglePTrackerHandle uvHealthTracker = std::make_shared<PercentageSingleHealthTracker>(*uiFactory.get(), "Ambiental Light", uvTracker, uvCalibrate,"UV Lamp");
	uvHealthTracker->setUpdates(uvCalibrateMin, uvCalibrateMax, uvDesireMin, uvDesireMax);


	HealthTrackerAction uvOnLower = [&outputHelper]()
	{
		LOG_INFO("Turning on UV Lamp...");
		outputHelper->startUv();
	};

	HealthTrackerAction uvOnDesired = [&outputHelper]()
	{
		LOG_INFO("Turning off UV Lamp...");
		outputHelper->stopUv();
	};

	uvHealthTracker->setOnLower(uvOnLower);
	uvHealthTracker->setOnDesired(uvOnDesired);



	FSettingUpdate humidityDesireMin = [&settingsHelper](const float value)
	{
		settingsHelper->data.humidity.desireMin = value;
		settingsHelper->save();
	};

	FSettingUpdate humidityDesireMax = [&settingsHelper](const float value)
	{
		settingsHelper->data.humidity.desireMax = value;
		settingsHelper->save();
	};

	FSettingUpdate temperatureDesireMin = [&settingsHelper](const float value)
	{
		settingsHelper->data.temperature.desireMin = value;
		settingsHelper->save();
	};

	FSettingUpdate temperatureDesireMax = [&settingsHelper](const float value)
	{
		settingsHelper->data.temperature.desireMax = value;
		settingsHelper->save();
	};

	Interval<float> temperatureDesire(settingsHelper->data.temperature.desireMin, settingsHelper->data.temperature.desireMax);
	Interval<float> temperatureCalibrate(settingsHelper->data.temperature.calibrateMin, settingsHelper->data.temperature.calibrateMax);

	SensorTracker<float> temperatureTracker(temperatureDesire, temperatureCalibrate);
	SingleTrackerHandle temperatureHealthTracker = std::make_shared<SingleHealthTracker>(*uiFactory.get(), "Ambiental Temperature", temperatureTracker, "\370C");



	Interval<float> humidityDesire(settingsHelper->data.humidity.desireMin, settingsHelper->data.humidity.desireMax);
	Interval<float> humidityCalibrate(settingsHelper->data.humidity.calibrateMin, settingsHelper->data.humidity.calibrateMax);

	SensorTracker<float> humidityTracker(humidityDesire,humidityCalibrate);
	SingleTrackerHandle humidityHealthTracker = std::make_shared<SingleHealthTracker>(*uiFactory.get(), "Ambiental Humidity", humidityTracker, "%");

	temperatureHealthTracker->setUpdates(temperatureDesireMin, temperatureDesireMax);
	humidityHealthTracker->setUpdates(humidityDesireMin, humidityDesireMax);


	auto inputHelper = std::make_shared<InputHelper>(soilHealthTracker,uvHealthTracker, temperatureHealthTracker, humidityHealthTracker,settingsHelper);



	std::shared_ptr<PlantHealthProvider> provider = std::make_shared<PlantHealthProvider>(mapper, controller, inputHelper,outputHelper);
	endpoint::ChannelParserDelegatorHandle channelDelegator = std::make_shared<endpoint::ChannelParserDelegator>(provider);
	parser::ParserDelegatorHandle delegator = endpoint::ParserDelegatorFactory::build(channelDelegator);
	channelDelegator->addDelegator(0, delegator);

	parser::MessageBuilderHandle builder = std::make_shared<parser::MessageBuilder>(socket, channelDelegator);


	uint32_t subscribeMsgId = mapper->registerType<service::SubscribeMessage>();


	auto sub = service::SubscribeSerializer::serialize(service::SubscribeMessage(subscribeMsgId, epName));

	control::endpoint::ProgressBarHandle overallBar = uiFactory->makeProgresBar("Overall Health", 0);
	controller->addControl(overallBar);

	OverallUpdate overall = [&overallBar,&inputHelper]()
	{
		overallBar->setProgress(inputHelper->getOverallHealth());
	};

	inputHelper->soilTracker->setOvearll(overall);
	inputHelper->uvTracker->setOvearll(overall);
	inputHelper->humidity->setOvearll(overall);
	inputHelper->temperature->setOvearll(overall);


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
