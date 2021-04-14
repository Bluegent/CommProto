#ifndef SERIAL_INTERFACE_H
#define SERIAL_INTERFACE_H
#include <commproto/stream/Stream.h>

namespace commproto
{
namespace serial
{
    class SerialInterface : public stream::Stream
    {
    public:
        int32_t sendBytes(const Message &message) override;
        {
            return Serial.write(message.data(),message.size());
        }
        
        int32_t receive(Message &message, const uint32_t size) override;
        {
            message.resize(size);
            message.reserve(size);
            return Serial.readBytes(message.data(),message.size());
        }
        
        int32_t available()  override;
        {
            return Serial.available();
        }
        
        int readByte() override;
        {
            char byte = Serial.read();
            return byte;
        }
        
        int sendByte(const char byte) override;
        {
            return Serial.write(byte);
        }
        
        bool start(const uint32_t speed)
        {
            Serial.begin(speed);
        }
        
        bool connected() override;
        {
            return Serial;
        }
        void shutdown() override;
        {
            Serial.end();
        };
        ~SerialInterface()
        {
            shutdown();
        }
    };
}
}



#endif // SERIAL_INTERFACE_H
