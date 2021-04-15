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
        int32_t receive(Message &message, const uint32_t size) override;     
        int32_t available()  override;     
        int readByte() override;      
        int sendByte(const char byte) override;      
        bool start(const int speed);
        bool connected() override;
        void shutdown() override;
        ~SerialInterface();
    };
    using SerialHandle = std::shared_ptr<SerialInterface>;
}
}



#endif // SERIAL_INTERFACE_H
