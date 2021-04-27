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

	static constexpr const char * const serverPort = "serverPort";
	static constexpr const int32_t defaultServerPort = 25565;

	static constexpr const char * const serverAddress = "serverAddress";
	static constexpr const char * const serverAddressDefault = "127.0.0.1";
};



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

	const int32_t port = commproto::config::getValueOrDefault(doc, ConfigValues::serverPort, ConfigValues::defaultServerPort);
	const char * const address = commproto::config::getValueOrDefault(doc, ConfigValues::serverAddress, ConfigValues::serverAddressDefault);

	LOG_INFO("Authentification service attempting to connect to %s:%d", address, port);
	commproto::sockets::SocketHandle socket = std::make_shared<commproto::sockets::SocketImpl>();
	if (!socket->initClient(address, port))
	{
		LOG_ERROR("A problem occurred while starting service, shutting down...");
		return 1;
	}

	LOG_INFO("Endpoint started...");

	commproto::serial::SerialHandle serial = std::make_shared<commproto::serial::SerialInterface>();

	LOG_INFO("Authentification service connecting to device %s on baudrate %d...", device, baud);
	if (!serial->start(device, SPEED))
	{
		LOG_ERROR("A problem occurred while starting authentification service, shutting down...");
		return 1;
	}

	LOG_INFO("Authentification service started");
	AuthServiceHandle service = std::make_shared<AuthServiceImpl>(serial,socket);

	service->initializeDispatch();

	service->initializeDevice();

	service->loopBlocking();

	return 0;
}
