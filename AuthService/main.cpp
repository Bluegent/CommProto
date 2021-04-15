#define PARSER_DELEGATOR_NO_WARNING

#include <commproto/service/Dispatch.h>
#include <SocketImpl.h>
#include <commproto/logger/Logging.h>	
#include <commproto/service/Connection.h>
#include <commproto/config/ConfigParser.h>
#include <commproto/logger/FileLogger.h>
#include <SerialInterface.h>
#include <commproto/logger/LoggingMessage.h>
#include <commproto/parser/ParserDelegatorUtils.h>
#include <commproto/parser/MessageBuilder.h>
#include <commproto/device/AuthChains.h>
#include <chrono>

#ifdef _WIN32
#define SPEED CBR_115200
#elif	
#include <termios.h>
#define SPEED B115200

#endif


namespace ConfigValues
{
	static constexpr const char * const baudRate = "baud";
	static constexpr const int32_t defaultBaudRate = 115200;

	static constexpr const char * const device = "device";
	static constexpr const char * const defaultDevice = "/dev/ttyUSB0";

	static constexpr const char * const logToConsole = "logToConsole";
	static constexpr const bool logToConsoleDefault = true;
};

struct APData
{
	std::string ssid;
	std::string name;
	std::string manufacturer;
	std::string description;
};


class AuthService
{
public:


	AuthService(const commproto::stream::StreamHandle stream_)
		: stream{ stream_ }
		, mapper{ commproto::messages::TypeMapperFactory::build(stream) }
		, scanning{ false }
		, scanId{ mapper->registerType<commproto::device::ScanForNetworksMessage>() }
		, authorizeId{ mapper->registerType<commproto::device::DeviceAuthAccept>() }
		, rejectId{ mapper->registerType<commproto::device::DeviceAuthReject>() }
	{

	}
	void scan()
	{
		if (scanning)
		{
			return;
		}
		scanning = true;
		commproto::Message scan = commproto::device::ScanForNetworksSerializer::serialize(std::move(commproto::device::ScanForNetworksMessage(scanId)));
		stream->sendBytes(scan);

	}

	void setScanFinished()
	{
		scanning = false;
	}

	void handleRequest(const APData& data)
	{
		LOG_INFO("Name: \"%s\" (ssid:%s)", data.name.c_str(),data.ssid.c_str());
		LOG_INFO("Manufacturer: \"%s\"", data.manufacturer.c_str());
		LOG_INFO("Description: \"%s\"", data.description.c_str());
		accept(data.ssid);
	}

	void accept(const std::string & name)
	{
		std::vector<std::string> props;
		props.push_back("CommProtoHub"); //ssid of hub
		props.push_back("commprotopassword"); //password for hub
		props.push_back("192.168.1.8"); //dispatch address
		uint32_t port = 25565; //dispatch port
		commproto::Message accept = commproto::device::DeviceAuthAcceptSerializer::serialize(std::move(commproto::device::DeviceAuthAccept(authorizeId,name,props,port)));
		stream->sendBytes(accept);
	}
	void reject(const std::string & name)
	{
		commproto::Message reject = commproto::device::DeviceAuthRejectSerializer::serialize(std::move(commproto::device::DeviceAuthReject(rejectId,name)));
		stream->sendBytes(reject);
	}


private:
	commproto::stream::StreamHandle stream;
	commproto::messages::TypeMapperHandle mapper;
	bool scanning;
	uint32_t scanId;
	uint32_t authorizeId;
	uint32_t rejectId;
};
using AuthServiceHandle = std::shared_ptr<AuthService>;

class DeviceReqHandler : public commproto::parser::Handler
{
public:
	DeviceReqHandler(const AuthServiceHandle & service_) : service{ service_ } {}
	void handle(commproto::messages::MessageBase&& data) override;
private:
	AuthServiceHandle service;
};

void DeviceReqHandler::handle(commproto::messages::MessageBase&& data)
{
	commproto::device::DeviceAuthRequestMessage& msg = static_cast<commproto::device::DeviceAuthRequestMessage&>(data);
	if (msg.prop.size() != 4)
	{
		LOG_ERROR("Incomplete amount of request parameters");
	}
	APData ap {msg.prop[0],msg.prop[1],msg.prop[2],msg.prop[3]};
	service->handleRequest(ap);
}



class ScanFinishedHandler : public commproto::parser::Handler
{
public:
	ScanFinishedHandler(const AuthServiceHandle & service_)
		:service{ service_ }
	{

	}
	void handle(commproto::messages::MessageBase&& data) override;
private:
	AuthServiceHandle service;
};

void ScanFinishedHandler::handle(commproto::messages::MessageBase&& data)
{
	service->setScanFinished();
}



commproto::parser::ParserDelegatorHandle build(const AuthServiceHandle & service)
{
	commproto::parser::ParserDelegatorHandle delegator = std::make_shared<commproto::parser::ParserDelegator>();
	commproto::parser::buildBase(delegator);
	commproto::parser::addParserHandlerPair<commproto::logger::LogParser, commproto::logger::LogMessage>(delegator, std::make_shared<commproto::logger::LogHandler>());
	commproto::parser::addParserHandlerPair<commproto::device::DeviceAuthRequestParser, commproto::device::DeviceAuthRequestMessage>(delegator, std::make_shared<DeviceReqHandler>(service));
	commproto::parser::addParserHandlerPair<commproto::device::ScanFinishedParser, commproto::device::ScanFinished>(delegator, std::make_shared<ScanFinishedHandler>(service));

	return delegator;
}


int main(int argc, const char * argv[]) {

	const char * configFile;
	if (argc <= 1)
	{
		configFile = "auth.cfg";
	}
	else
	{
		configFile = argv[1];
	}

	rapidjson::Document doc = commproto::config::ConfigParser(configFile).get();

	const int32_t baud = commproto::config::getValueOrDefault(doc, ConfigValues::baudRate, ConfigValues::defaultBaudRate);
	bool logToConsole = commproto::config::getValueOrDefault(doc, ConfigValues::logToConsole, ConfigValues::logToConsoleDefault);
	const char * const device = commproto::config::getValueOrDefault(doc, ConfigValues::device, ConfigValues::defaultDevice);


	commproto::logger::FileLogger logger("auth_log_" + commproto::logger::FileLogger::getTimestamp() + ".txt");
	if (!logToConsole)
	{
		logger.open();
		commproto::logger::setLoggable(&logger);
	}


	commproto::serial::SerialHandle serial = std::make_shared<commproto::serial::SerialInterface>();

	LOG_INFO("Authentification service connecting to device %s on baudrate %d...", device, baud);
	if (!serial->start(device, SPEED))
	{
		LOG_ERROR("A problem occurred while starting authentification service, shutting down...");
		return 1;
	}

	LOG_INFO("Authentification service started");
	int res = serial->sendByte(sizeof(void*));
	if (res != 1)
	{
		LOG_ERROR("An issue occurred when sending sizeof pointer on host system");
	}
	auto service = std::make_shared<AuthService>(serial);
	auto delegator = build(service);
	auto builder = std::make_shared<commproto::parser::MessageBuilder>(serial, delegator);

	do
	{
		builder->pollAndReadTimes(100);
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	} while (!delegator->hasMapping<commproto::logger::LogMessage>());


	auto now = std::chrono::system_clock::now();
	auto then = std::chrono::system_clock::now();
	service->scan();
	const uint32_t periodicScan = 120000;
	while (true)
	{
		builder->pollAndReadTimes(100);
		std::this_thread::sleep_for(std::chrono::milliseconds(1));

		then = std::chrono::system_clock::now();
		auto diff = then - now;
		if (std::chrono::duration_cast<std::chrono::milliseconds>(diff).count() >= periodicScan)
		{
			service->scan();
			now = then;
		}

	}
	return 0;
}
