#ifndef LOGGING_MESSAGE_H
#define LOGGING_MESSAGE_H
#include <commproto/messages/DoublePropertyChain.h>
#include <commproto/logger/Logging.h>


namespace commproto
{
    namespace logger
    {
		MAKE_DOUBLE_PROP_MESSAGE(LogMessage,std::string,std::string);
		using LogParser = messages::DoublePropertyParser<std::string,std::string>;
		using LogSerializer = messages::DoublePropertySerializer<std::string,std::string>;


		class LogHandler : public parser::Handler
    	{
		public:
			void handle(messages::MessageBase&& data) override;
		};

	    inline void LogHandler::handle(messages::MessageBase&& data)
	    {
			LogMessage& msg = static_cast<LogMessage&>(data);

			LOG_SOURCE("[R][%s]:%s\n",msg.prop.c_str(),msg.prop2.c_str());
	    }
    }

	DEFINE_DATA_TYPE(logger::LogMessage);
}

#endif 