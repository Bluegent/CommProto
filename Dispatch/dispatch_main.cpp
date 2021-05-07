#define PARSER_DELEGATOR_NO_WARNING

#include <commproto/service/Dispatch.h>
#include <SocketImpl.h>
#include <commproto/logger/Logging.h>
#include <commproto/service/Connection.h>
#include <commproto/config/ConfigParser.h>
#include "commproto/logger/FileLogger.h"



namespace ConfigValues
{
    static constexpr const char * const serverPort = "port";
    static constexpr const int32_t defaultServerPort = 25565;

    static constexpr const char * const serverAddress = "serverAddress";
    static constexpr const char * const serverAddressDefault = "127.0.0.1";

    static constexpr const char * const logToConsole = "logToConsole";
	static constexpr const bool logToConsoleDefault = true;
};


int main(int argc, const char * argv[])
{

    const char * configFile;
    if(argc <= 1)
    {
        configFile = "dispatch.cfg";
    }
    else
    {
        configFile = argv[1];
    }

    rapidjson::Document doc = commproto::config::ConfigParser(configFile).get();

    const int32_t port = commproto::config::getValueOrDefault(doc, ConfigValues::serverPort, ConfigValues::defaultServerPort);
	bool logToConsole = commproto::config::getValueOrDefault(doc, ConfigValues::logToConsole, ConfigValues::logToConsoleDefault);
    const char * const address = commproto::config::getValueOrDefault(doc, ConfigValues::serverAddress, ConfigValues::serverAddressDefault);


	commproto::logger::FileLogger logger("dispatch_log_" + commproto::logger::FileLogger::getTimestamp() + ".txt");
	if (!logToConsole)
	{
		logger.open();
		commproto::logger::setLoggable(&logger);
	}

	commproto::service::Dispatch dsp;
	commproto::sockets::SocketHandle socket = std::make_shared<commproto::sockets::SocketImpl>();

    LOG_INFO("Dispatch server starting on %s:%d...",address,port);
    if (!socket->initServer(address,port))
	{
		LOG_ERROR("A problem occurred while starting dispatch service, shutting down...");
		return 1;
	}


	dsp.startCheckingConnections();

    LOG_INFO("Dispatch server started on %s:%d, wating for new connection...",address,port);

	while (true) {
		commproto::sockets::SocketHandle newCon = socket->acceptNext();
		dsp.addConnection(newCon);
	}
	return 0;
}
