#include <SocketImpl.h>
#include <commproto/logger/Logging.h>
#include <commproto/messages/SenderMaping.h>
#include <commproto/service/ServiceChains.h>
#include <commproto/messages/TypeMapper.h>
#include <commproto/endpoint/ChannelParserDelegator.h>
#include <commproto/parser/ParserDelegatorUtils.h>
#include <commproto/endpoint/ParserDelegatorFactory.h>
#include <commproto/control/endpoint/ParserDelegatorUtils.h>
#include <commproto/config/ConfigParser.h>
#include <commproto/parser/MessageBuilder.h>
#include <plant/interface/PlantMessages.h>
#include <thread>
#include <random>

namespace ConfigValues
{
	static constexpr const char * const serverPort = "serverPort";
	static constexpr const int32_t defaultServerPort = 25565;

	static constexpr const char * const serverAddress = "serverAddress";
	static constexpr const char * const serverAddressDefault = "127.0.0.1";
}


using namespace commproto;
using namespace service;

class UvHandler : public parser::Handler
{
public:
	void handle(messages::MessageBase&& data) override;
};

void UvHandler::handle(messages::MessageBase&& data)
{
	auto msg = static_cast<plant::ToggleUVLamp&>(data);
	LOG_INFO("Toggling UV Lamp %s", msg.prop ? "on" : "off");
}

class PumpHandler : public parser::Handler
{
public:
	void handle(messages::MessageBase&& data) override;
};

void PumpHandler::handle(messages::MessageBase&& data)
{
	auto msg = static_cast<plant::TogglePump&>(data);
	LOG_INFO("Toggling Pump %s", msg.prop ? "on" : "off");
}

parser::ParserDelegatorHandle buildSelfDelegator()
{
	std::shared_ptr<parser::ParserDelegator> delegator = std::make_shared<parser::ParserDelegator>();
	parser::DelegatorUtils::buildBase(delegator);
	parser::DelegatorUtils::addParserHandlerPair<plant::TogglePumpParser,plant::TogglePump>(delegator,std::make_shared<PumpHandler>());
	parser::DelegatorUtils::addParserHandlerPair<plant::ToggleUVLampParser,plant::ToggleUVLamp>(delegator,std::make_shared<UvHandler>());
	return delegator;
}






class PlantTesterProvider : public endpoint::DelegatorProvider {
public:
	PlantTesterProvider(const messages::TypeMapperHandle & mapper_)
		: mapper{ mapper_ }
	{

	}
	parser::ParserDelegatorHandle provide(const std::string& name, const uint32_t id) override
	{
		parser::ParserDelegatorHandle delegator = buildSelfDelegator();
		
		return delegator;
	}
private:
	messages::TypeMapperHandle mapper;
	control::endpoint::UIControllerHandle controller;
};

const uint32_t top = 4096;
std::random_device randDevice;
std::default_random_engine engine(randDevice());
std::uniform_int_distribution<int32_t> uniform_dist(-50, 100);

void ranMod_int(uint32_t & target)
{
	int32_t ran = uniform_dist(engine);

	if(ran < 0 && ran *-1 > static_cast<int32_t>(target))
	{
		target = 0;
	} 
	else
	{
		target += ran;
	}
	if(target > top)
	{
		target = top;
	}
}

void ranMod_float(float & target, const float min, const float max)
{
	float ran = uniform_dist(engine);
	target += ran / 50.f;
	if(target < min)
	{
		target = min;
	}
	if (target > max)
	{
		target = max;
	}
}


int main(int argc, const char * argv[])
{

	const char * configFile;
	if (argc <= 1)
	{
		configFile = "planttester.cfg";
	}
	else
	{
		configFile = argv[1];
	}

	rapidjson::Document doc = commproto::config::ConfigParser(configFile).get();

	const int32_t port = commproto::config::getValueOrDefault(doc, ConfigValues::serverPort, ConfigValues::defaultServerPort);
	const char * const address = commproto::config::getValueOrDefault(doc, ConfigValues::serverAddress, ConfigValues::serverAddressDefault);

	LOG_INFO("Attempting to connect to %s:%d", address, port);
	SenderMapping::InitializeName("Endpoint::PlantTester");
	sockets::SocketHandle socket = std::make_shared<sockets::SocketImpl>();
	if (!socket->initClient(address, port))
	{
		LOG_ERROR("A problem occurred while starting plant tester, shutting down...");
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


	//delegator to parse incoming messages	

	std::shared_ptr<PlantTesterProvider> provider = std::make_shared<PlantTesterProvider>(mapper);
	endpoint::ChannelParserDelegatorHandle channelDelegator = std::make_shared<endpoint::ChannelParserDelegator>(provider);
	parser::ParserDelegatorHandle delegator = endpoint::ParserDelegatorFactory::build(channelDelegator);
	channelDelegator->addDelegator(0, delegator);
	parser::MessageBuilderHandle builder = std::make_shared<parser::MessageBuilder>(socket, channelDelegator);


	uint32_t tempMsgId = mapper->registerType<plant::TempHum>();
	uint32_t soilMsgId = mapper->registerType<plant::Soil>();
	uint32_t uvMsgId = mapper->registerType<plant::UvLight>();

	
	//wait until we are sure we have an ID
	do
	{
		builder->pollAndRead();
	} while (SenderMapping::getId() == 0);

	auto now = std::chrono::system_clock::now().time_since_epoch();;
	auto then = std::chrono::system_clock::now().time_since_epoch();;


	float temperature = 25.f;
	float humidity = 50;

	uint32_t uv = 0;
	uint32_t soil = 0;

	while (true)
	{
		now =  std::chrono::system_clock::now().time_since_epoch();
		if (std::chrono::duration_cast<std::chrono::seconds>(now - then).count() > 3)
		{
			then = now;
			ranMod_int(soil);
			ranMod_int(uv);
			ranMod_float(temperature, -5.f, 50.f);
			ranMod_float(humidity, 0.f, 100.f);

			LOG_INFO("Soil Humidity: %d, UV: %d, Temperature: %.2f C, Humidity: %.2f%%",soil,uv,temperature,humidity);

					
			socket->sendBytes(plant::TempHumSerializer::serialize(plant::TempHum(tempMsgId, temperature, humidity)));
			socket->sendBytes(plant::SoilSerializer::serialize(plant::Soil(soilMsgId, soil)));
			socket->sendBytes(plant::UvLightSerializer::serialize(plant::UvLight(uvMsgId, uv)));
		

		}
		builder->pollAndReadTimes(100);
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
	socket->shutdown();
	return 0;
}
