#ifndef SERIAL_INTERFACE_H
#define SERIAL_INTERFACE_H
#include <windows.h>
#include <commproto/sockets/Socket.h>

namespace commproto
{
namespace serial
{

    class SerialInterface : public sockets::Socket
    {
    public:
        SerialInterface();
        int32_t sendBytes(const Message &message) override;
        int32_t receive(Message &message, const uint32_t size) override;
        int32_t pollSocket() override;
        int readByte() override;
        int sendByte(const char byte) override;
        bool initClient(const std::string &addr, const uint32_t port) override;
        bool initServer(const std::string &addr, const uint32_t port) override;
        sockets::SocketHandle acceptNext() override;
        bool connected() override;
        void shutdown() override;
        void setTimeout(const uint32_t msec) override;
        ~SerialInterface();
    private:
		HANDLE serialPort;
    };
}
}



#endif // SERIAL_INTERFACE_H
