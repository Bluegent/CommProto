#include <commproto/serial/LogToSerial.h>
#include <commproto/logger/LoggingMessage.h>


namespace commproto
{
    namespace serial
    {
        LogToSerial::LogToSerial(const serial::SerialHandle& serial, const uint32_t logMsgId)
            : serial(serial)
            , logMsgId(logMsgId)
        {
        }

        void LogToSerial::addLog(const char* line, const uint32_t size)
        {
            if (!serial)
            {
                return;
            }
            std::string message(line, size);
            Message log = logger::LogSerializer::serialize(std::move(logger::LogMessage(logMsgId, "AuthDevice", message)));
            serial->sendBytes(log);
        }
    }
}
