#ifndef SERIAL_INTERFACE_H
#define SERIAL_INTERFACE_H
#include <commproto/Stream/Stream.h>

namespace commproto
{
namespace serial
{
    class SerialInterface : public stream::Stream
    {
    public:
        SerialInterface();
        int32_t sendBytes(const Message &message) override;
        int32_t receive(Message &message, const uint32_t size);
        int32_t available() override;
        int readByte() override;
        int sendByte(const char byte) override;
        bool start(const std::string &addr, const uint32_t port);
        bool connected() override;
        void shutdown() override;
        ~SerialInterface();
    private:
        int serialPort;
    };
    
    using SerialHandle = std::shared_ptr<SerialInterface>;
}
}



#endif // SERIAL_INTERFACE_H
