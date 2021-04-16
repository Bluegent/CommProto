#ifndef LOG_TO_SERIAL_H
#define LOG_TO_SERIAL_H
#include <commproto/logger/Logging.h>
#include <SerialInterface.h>

namespace commproto
{
    namespace serial
    {
        class LogToSerial : public logger::Loggable
        {
        public:
            LogToSerial(const SerialHandle & serial, const uint32_t logMsgId);
            void addLog(const char* line, const uint32_t size) override;
            void close() override {}
            void open() override {}
        private:
            SerialHandle serial;
            const uint32_t logMsgId;
        };
        using LogToSerialHandle = std::shared_ptr<LogToSerial>;
    }
}
#endif //LOG_TO_SERIAL_H