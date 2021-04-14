#ifndef COMMPROTO_SOCKET_IMPL_H
#define COMMPROTO_SOCKET_IMPL_H

#include <commproto/sockets/Socket.h>
#include <winsock2.h>

namespace commproto {
namespace sockets {

	enum class WsaStartupResult
	{
		AlreadyStarted,
		StartupSucceeded,
		StartupFailed
	};

    enum class Mode : uint8_t {
        Unassigned,
        Server,
        Client
    };

    class SocketImpl : public Socket {
    public:
        SocketImpl();
		~SocketImpl();

	    int32_t sendBytes(const Message& message) override;
	    int32_t receive(Message& message, uint32_t size) override;
        bool initClient(const std::string& addr, const uint32_t port) override;
        bool initServer(const std::string& addr,const uint32_t port) override;
        SocketHandle acceptNext() override; // blocking
	    int32_t available() override;
        int readByte() override;
        int sendByte(const char byte) override;
		bool connected() override;
		void shutdown() override;
		void setTimeout(const uint32_t msec) override;

    private:
        //returns a ready socket from acceptNext()
        SocketImpl(SOCKET handle, const Mode mode, const bool initialized);
        Mode socketMode;
        bool isInitialized;
		SOCKET socketHandle;

	    static WsaStartupResult startWsa();
	    static void stopWsa();
		static bool isWsaStarted;

		bool isConnected;
    };
} // namespace socket
} // namespace commproto

#endif
