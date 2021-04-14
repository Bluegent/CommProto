#ifndef COMMPROTO_UNIX_SOCKETIMPL_H
#define COMMPROTO_UNIX_SOCKETIMPL_H

#include <commproto/sockets/Socket.h>

namespace commproto {
namespace sockets {

    enum class Mode : uint8_t {
        Unassigned,
        Server,
        Client
    };

    class SocketImpl : public Socket {
    public:
        SocketImpl();

        int32_t sendBytes(const Message& message) override;
        int32_t receive(Message& message, uint32_t size) override;
        bool initClient(const std::string& addr, const uint32_t port) override;
        bool initServer(const std::string& addr, const uint32_t port) override;
        SocketHandle acceptNext() override; // blocking
        int32_t available() override;
        int readByte() override;
        int sendByte(const char byte) override;
        bool connected() override;
        void shutdown() override;
        void setTimeout(const uint32_t msec) override;

        ~SocketImpl();

    private:
        //returns a ready socket from acceptNext()
        SocketImpl(const int32_t handle, const Mode mode, const bool initialized);
        bool initFd();
        Mode socketMode;
        bool isInitialized;
        int32_t socketHandle;
    };
} // namespace socket
} // namespace commproto
#endif
