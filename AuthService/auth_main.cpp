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

#include "AuthServiceImpl.h"
#include "AuthServiceHandlers.h"

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

commproto::parser::ParserDelegatorHandle build(const AuthServiceHandle & service)
{
	commproto::parser::ParserDelegatorHandle delegator = std::make_shared<commproto::parser::ParserDelegator>();
	commproto::parser::DelegatorUtils::buildBase(delegator);
	commproto::parser::DelegatorUtils::addParserHandlerPair<commproto::logger::LogParser, commproto::logger::LogMessage>(delegator, std::make_shared<commproto::logger::LogHandler>());
	commproto::parser::DelegatorUtils::addParserHandlerPair<commproto::device::DeviceAuthRequestParser, commproto::device::DeviceAuthRequestMessage>(delegator, std::make_shared<DeviceReqHandler>(service));
	commproto::parser::DelegatorUtils::addParserHandlerPair<commproto::device::ScanFinishedParser, commproto::device::ScanFinished>(delegator, std::make_shared<ScanFinishedHandler>(service));
	commproto::parser::DelegatorUtils::addParserHandlerPair<commproto::device::KeepAliveParser, commproto::device::KeepAlive>(delegator, std::make_shared<KeepAliveHandler>(service));
	commproto::parser::DelegatorUtils::addParserHandlerPair<commproto::device::ScanProgressParser, commproto::device::ScanProgress>(delegator, std::make_shared<ScanProgressHandler>(service));
	commproto::parser::DelegatorUtils::addParserHandlerPair<commproto::device::ScanStartedParser, commproto::device::ScanStarted>(delegator, std::make_shared<ScanStartHandler>(service));

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
	auto service = std::make_shared<AuthServiceImpl>(serial);
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
