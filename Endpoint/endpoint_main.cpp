#include <SocketImpl.h>
#include <commproto/logger/Logging.h>
#include <commproto/messages/SenderMaping.h>
#include <commproto/service/ServiceChains.h>
#include <commproto/messages/TypeMapper.h>
#include <commproto/endpoint/ChannelParserDelegator.h>
#include <commproto/parser/ParserDelegatorUtils.h>
#include <commproto/endpoint/ParserDelegatorFactory.h>
#include <commproto/control/endpoint/UIFactory.h>
#include <commproto/control/endpoint/ParserDelegatorUtils.h>
#include <commproto/control/endpoint/Label.h>
#include <sstream>
#include <commproto/config/ConfigParser.h>
#include <commproto/parser/MessageBuilder.h>
#include <thread>

namespace ConfigValues
{
	static constexpr const char * const serverPort = "serverPort";
	static constexpr const int32_t defaultServerPort = 25565;

	static constexpr const char * const serverAddress = "serverAddress";
	static constexpr const char * const serverAddressDefault = "127.0.0.1";
}


using namespace commproto;
using namespace service;


parser::ParserDelegatorHandle buildSelfDelegator()
{
	std::shared_ptr<parser::ParserDelegator> delegator = std::make_shared<parser::ParserDelegator>();
	parser::DelegatorUtils::buildBase(delegator);
	return delegator;
}


class EndpointProvider : public endpoint::DelegatorProvider {
public:
	EndpointProvider(const messages::TypeMapperHandle & mapper_, const control::endpoint::UIControllerHandle & controller_)
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

int main(int argc, const char * argv[])
{

	const char * configFile;
	if (argc <= 1)
	{
		configFile = "endpoint.cfg";
	}
	else
	{
		configFile = argv[1];
	}

	rapidjson::Document doc = commproto::config::ConfigParser(configFile).get();

	const int32_t port = commproto::config::getValueOrDefault(doc, ConfigValues::serverPort, ConfigValues::defaultServerPort);
	const char * const address = commproto::config::getValueOrDefault(doc, ConfigValues::serverAddress, ConfigValues::serverAddressDefault);

	LOG_INFO("Attempting to connect to %s:%d", address, port);
	SenderMapping::InitializeName("Endpoint::Simulator");
	sockets::SocketHandle socket = std::make_shared<sockets::SocketImpl>();
	if (!socket->initClient(address, port))
	{
		LOG_ERROR("A problem occurred while starting endpoint, shutting down...");
		return 1;
	}

	LOG_INFO("Endpoint started...");

	uint8_t attempt = 0;
	uint8_t maxAttempt = 1;

	//send ptr size
	socket->sendByte(sizeof(void*));


	messages::TypeMapperHandle mapper = messages::TypeMapperFactory::build(socket);

	//registering our channel name
	uint32_t registerId = mapper->registerType<RegisterChannelMessage>();
	RegisterChannelMessage nameMsg(registerId, SenderMapping::getName());
	Message nameSerialized = RegisterChannelSerializer::serialize(std::move(nameMsg));
	socket->sendBytes(nameSerialized);


	float increment = 0.01f;
	float direction = 1;

	//delegator to parse incoming messages
	auto uiFactory = std::make_shared<control::endpoint::UIFactory>("myUI", mapper, socket);
	control::endpoint::UIControllerHandle controller = uiFactory->makeController();

	auto toggleIncrement = uiFactory->makeToggle("Toggle increment 0.x/x.0", [&increment](bool state)
	{
		LOG_INFO("MyToggle state switched: %s", state ? "True" : "False");
		if (state)
		{
			increment = 1.0f;
		}
		else
		{
			increment = 0.01;
		}
	});
	controller->addControl(toggleIncrement);

	auto notif = uiFactory->makeNotification("You just pressed the notification button.");
	notif->addOption("Oh... okay?");

	auto genericAction = [](const std::string& option)
	{
		LOG_INFO("Generic notification response with option :\"%s\"", option.c_str());
	};

	controller->addNotification(notif);

	auto notifButton = uiFactory->makeButton("Send me a notification", [&direction, &controller, &notif, &genericAction]()
	{
		LOG_INFO("MyButton has been pressed");
		controller->displayNotification(notif->getId(),"Yes this is a notification :)", genericAction);

	});
	controller->addControl(notifButton);

	control::endpoint::SliderAction slAction = [](float value)
	{
		LOG_INFO("Desired temp: %.2f", value);
	};
	auto slider = uiFactory->makeSlider("Desired temperature", slAction);
	slider->setLimits(5.f, 40.f);
	slider->setStep(0.5f);
	slider->setInitialValue(24.f);
	controller->addControl(slider);

	control::endpoint::LabelHandle tempLabel = uiFactory->makeLabel("Temperature", "0.00 C");
	controller->addControl(tempLabel);

	std::shared_ptr<EndpointProvider> provider = std::make_shared<EndpointProvider>(mapper, controller);
	endpoint::ChannelParserDelegatorHandle channelDelegator = std::make_shared<endpoint::ChannelParserDelegator>(provider);
	parser::ParserDelegatorHandle delegator = endpoint::ParserDelegatorFactory::build(channelDelegator);
	channelDelegator->addDelegator(0, delegator);

	parser::MessageBuilderHandle builder = std::make_shared<parser::MessageBuilder>(socket, channelDelegator);

	//wait until we are sure we have an ID
	do
	{
		builder->pollAndRead();
	} while (SenderMapping::getId() == 0);

	for (uint32_t index = 0; index < 100; ++index)
	{
		builder->pollAndRead();
	}

	int counter = 0;
	int updateCounter = 2000;
	float temp = 0.00f;
	
	while (true)
	{
		builder->pollAndReadTimes(100);
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
		++counter;
		if (counter == updateCounter)
		{
			
			temp += direction * increment;
			
			std::stringstream tempStr;
			tempStr.precision(3);
			tempStr << temp << " C";
			tempLabel->setText(tempStr.str());
			counter = 0;
		}
	}
    socket->shutdown();
	return 0;
}
