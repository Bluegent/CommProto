#ifndef COMMPROTO_SOCKET_H
#define COMMPROTO_SOCKET_H
#include <commproto/common/Common.h>
#include <commproto/stream/Stream.h>
#include <cstdint>
#include <string>
#include <vector>
#include <memory>

namespace commproto {
namespace sockets {


    class Socket;
    using SocketHandle = std::shared_ptr<Socket>;
    class Socket  : public stream::Stream {
    public:
	    virtual ~Socket() = default;   

        //initialize the socket as a client and connect it to [addr]:[port]
        virtual bool initClient(const std::string& addr, const uint32_t port) = 0;

        //initialize the socket as a server and bind it to [port]
        virtual bool initServer(const std::string& addr, const uint32_t port) = 0;

        //block the thread until a connection is available, then accept it
        virtual SocketHandle acceptNext() = 0;

		//set the timeout for socket operation
		virtual void setTimeout(const uint32_t msec) = 0;
    };

} // namespace core
} // namespace commproto

#endif //COMMPROTO_SOCKET_H
