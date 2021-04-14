#ifndef SERIAL_INTERFACE_H
#define SERIAL_INTERFACE_H
#include <commproto/sockets/Socket.h>

namespace commproto
{
namespace serial
{
    class SerialInterface : public sockets::Socket
    {
    public:
        int32_t sendBytes(const Message &message)
        {
            return Serial.write(message.data(),message.size());
        }
        int32_t receive(Message &message, const uint32_t size)
        {
            message.resize(size);
            message.reserve(size);
            return Serial.readBytes(message.data(),message.size());
        }
        int32_t pollSocket()
        {
            return Serial.available();
        }
        int readByte()
        {
            char byte = Serial.read();
            return byte;
        }
        int sendByte(const char byte)
        {
            return Serial.write(byte);
        }
        bool initClient(const std::string &addr, const uint32_t speed)
        {
            Serial.begin(speed);
        }
        bool initServer(const std::string &addr, const uint32_t port){};
        sockets::SocketHandle acceptNext(){return nullptr;}
        bool connected()
        {
            return Serial;
        }
        void shutdown()
        {
            Serial.end();
        };
        void setTimeout(const uint32_t msec){};
        ~SerialInterface()
        {
            shutdown();
        }
    };
}
}



#endif // SERIAL_INTERFACE_H
