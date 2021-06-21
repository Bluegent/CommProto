
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
#include <sstream>
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





class PercentageSingleHealthTracker
{

public:

	void setValue(const uint32_t value)
	{
		tracker.value.setValue(value);
		updateLabels();
	}

	void updateLabels()
	{
		tracker.value.calcPercentage();

		if (scoreLabel)
		{
			uint32_t score = tracker.getScore();
			std::stringstream scoreStream;
			scoreStream << score;
			scoreLabel->setText(scoreStream.str());
		}

		if (valueLabel)
		{
			float percentage = tracker.value.getPercentage();
			std::stringstream valueStream;
			valueStream.precision(3);
			valueStream << percentage << "%(" << tracker.value.getValue() << ")";
			valueLabel->setText(valueStream.str());
		}

	}

	void calibrateMin(const float value)
	{
		tracker.total.left = static_cast<uint32_t>(value);
		updateLabels();
	}

	void calibrateMax(const float value)
	{
		tracker.total.right = static_cast<uint32_t>(value);
		updateLabels();
	}


	void setDesiredMin(const float value)
	{
		tracker.desired.left = value;
		updateLabels();
	}


	void setDesiredMax(const float value)
	{
		tracker.desired.right = value;
		updateLabels();
	}

	void toggleCalibrationF(const bool state) const
	{
		if (minCalibration)
		{
			controller->setControlShownState(minCalibration->getId(), state);
		}
		if (maxCalibration)
		{
			controller->setControlShownState(maxCalibration->getId(), state);
		}
	}

	PercentageSingleHealthTracker(control::endpoint::UIFactory& factory, const std::string & name, const PercentageSensorTracker & tracker_)
		: tracker(tracker_)
	{


		control::endpoint::ToggleAction toggleCalibrationAct = [&, this](bool state)
		{
			this->toggleCalibrationF(state);
		};

		toggleCalibration = factory.makeToggle(name + " Calibration", toggleCalibrationAct);



		control::endpoint::SliderAction calibrateMinAct = [&, this](float value)
		{
			this->calibrateMin(value);
		};
		minCalibration = factory.makeSlider(name + " - Calibrate Min", calibrateMinAct);
		minCalibration->setInitialValue(tracker.value.absolute.left);
		minCalibration->setLimits(tracker.value.absolute.left, tracker.value.absolute.right);
		minCalibration->setStep(1);
		minCalibration->setDisplayState(false);



		control::endpoint::SliderAction calibrateMaxAct = [&, this](float value)
		{
			this->calibrateMax(value);
		};
		maxCalibration = factory.makeSlider(name + " - Calibrate Max", calibrateMaxAct);
		maxCalibration->setInitialValue(tracker.value.absolute.right);
		maxCalibration->setLimits(tracker.value.absolute.left, tracker.value.absolute.right);
		maxCalibration->setStep(1);
		maxCalibration->setDisplayState(false);


		control::endpoint::SliderAction desiredMinAct = [&, this](float value)
		{
			this->setDesiredMin(value);
		};
		minSlider = factory.makeSlider("Desired Min " + name, desiredMinAct, "%");
		minSlider->setInitialValue(tracker.desired.left);
		minSlider->setLimits(0, 100);
		minSlider->setStep(.5);



		control::endpoint::SliderAction desiredMaxAct = [&, this](float value)
		{
			this->setDesiredMax(value);
		};
		maxSlider = factory.makeSlider("Desired Max " + name, desiredMaxAct, "%");
		maxSlider->setInitialValue(tracker.desired.right);
		maxSlider->setLimits(0, 100);
		maxSlider->setStep(.5);

		valueLabel = factory.makeLabel(name, "");
		scoreLabel = factory.makeLabel(name + " Score", "");
		updateLabels();


		controller = factory.makeController();
		controller->addControl(toggleCalibration);
		controller->addControl(minCalibration);
		controller->addControl(maxCalibration);
		controller->addControl(minSlider);
		controller->addControl(maxSlider);
		controller->addControl(valueLabel);
		controller->addControl(scoreLabel);

	}


private:
	control::endpoint::UIControllerHandle controller;
	control::endpoint::ToggleHandle toggleCalibration;
	control::endpoint::SliderHandle minCalibration;
	control::endpoint::SliderHandle maxCalibration;

	control::endpoint::SliderHandle minSlider;
	control::endpoint::SliderHandle maxSlider;

	control::endpoint::LabelHandle valueLabel;
	control::endpoint::LabelHandle scoreLabel;

	PercentageSensorTracker tracker;
};

class SoilHandler : public parser::Handler
{
public:
	SoilHandler(PercentageSingleHealthTracker & tracker_) : tracker(tracker_) {}
	void handle(messages::MessageBase&& data) override;
private:
	PercentageSingleHealthTracker & tracker;
};

void SoilHandler::handle(messages::MessageBase&& data)
{
	auto msg = static_cast<plant::Soil&>(data);
	tracker.setValue(msg.prop);
}


class InputHelper
{
public:
	explicit InputHelper(PercentageSingleHealthTracker& soil_tracker)
		: soilTracker(soil_tracker)
	{
	}

	PercentageSingleHealthTracker soilTracker;
};

using InputHelperHandle = std::shared_ptr<InputHelper>;


parser::ParserDelegatorHandle buildSelfDelegator()
{
	std::shared_ptr<parser::ParserDelegator> delegator = std::make_shared<parser::ParserDelegator>();
	parser::DelegatorUtils::buildBase(delegator);
	return delegator;
}


class PlantHealthProvider : public endpoint::DelegatorProvider {
public:
	PlantHealthProvider(const messages::TypeMapperHandle & mapper_, const control::endpoint::UIControllerHandle & controller_, const InputHelperHandle & helper_)
		: mapper{ mapper_ }
		, controller{ controller_ }
		, helper(helper_)
	{

	}
	parser::ParserDelegatorHandle provide(const std::string& name, const uint32_t id) override
	{
		parser::ParserDelegatorHandle delegator = buildSelfDelegator();
		control::endpoint::DelegatorUtils::addParsers(delegator, controller);
		parser::DelegatorUtils::addParserHandlerPair<plant::SoilParser,plant::Soil>(delegator,std::make_shared<SoilHandler>(helper->soilTracker));

		return delegator;
	}
private:
	messages::TypeMapperHandle mapper;
	control::endpoint::UIControllerHandle controller;
	InputHelperHandle helper;
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
	PercentageSingleHealthTracker soilHealthTracker(*uiFactory.get(), "Soil Humidity", soilTracker);
	auto inputHelper = std::make_shared<InputHelper>(soilHealthTracker);


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
