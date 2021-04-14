#include "../interface/SocketImpl.h"
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>
#include <commproto/logger/Logging.h>
#include <errno.h>

namespace commproto {
namespace sockets {
    SocketImpl::SocketImpl()
        : socketMode(Mode::Unassigned)
        , socketHandle(-1)

    {
    }

    SocketImpl::SocketImpl(const int32_t handle, const Mode mode, const bool initialized)
        : socketMode(mode)
        , isInitialized(initialized)
        , socketHandle(handle)
    {
    }

    int32_t SocketImpl::sendBytes(const Message& message)
    {
        if (socketHandle < 0 || !isInitialized) {
            return 0;
        }
        return write(socketHandle, message.data(), message.size());
    }

    int32_t SocketImpl::receive(Message& message, const uint32_t size)
    {
        if (socketHandle < 0 || !isInitialized) {
            return 0;
        }
        message.reserve(size);
        return read(socketHandle, message.data(), size);
    }

    int SocketImpl::readByte()
    {
        if (socketHandle < 0 || !isInitialized) {
            return 0;
        }
        char output;
        read(socketHandle, &output, 1);
        return output;
    }

    int SocketImpl::sendByte(const char byte)
    {
        if (socketHandle < 0 || !isInitialized) {
            return 0;
        }
        return write(socketHandle, &byte,1);
    }

    bool SocketImpl::connected()
    {
        //TODO: implement me
        return true;
    }

    void SocketImpl::shutdown()
    {
        if (socketHandle < 0 || !isInitialized) {
            return;
        }
        close(socketHandle);
        socketHandle = 0;
    }

    void SocketImpl::setTimeout(const uint32_t msec)
    {
        //TODO: implement me
    }

    SocketImpl::~SocketImpl()
    {
        shutdown();
    }

    bool SocketImpl::initClient(const std::string& addr, uint32_t port)
    {
        if (socketMode != Mode::Unassigned) {
            return false;
        }
        socketMode = Mode::Client;
        bool success = initFd();
        if (!success) {
            return false;
        }
        //make a host entity of the address we are trying to connect to
        struct hostent* server = gethostbyname(addr.c_str());

        if (server == NULL) {
            return false;
        }
        struct sockaddr_in serv_addr;

        //initialize our address object
        serv_addr.sin_family = AF_INET;
        //copy server address from the produced server host entity
        bcopy((char*)server->h_addr,
            (char*)&serv_addr.sin_addr.s_addr,
            server->h_length);
        serv_addr.sin_port = htons(port);
        // try to connect
        isInitialized = connect(socketHandle, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == 0;
        if(!isInitialized)
        {
            LOG_ERROR("Error: %s(code:%d)",strerror(errno),errno);
        }
        return isInitialized;
    }

    bool SocketImpl::initServer(const std::string & addr_s, uint32_t port)
    {
        if (socketMode != Mode::Unassigned) {
            return false;
        }
        socketMode = Mode::Server;
        bool success = initFd();
        if (!success) {
            return false;
        }
        //make our address data
        struct sockaddr_in serv_addr;
        serv_addr.sin_family = AF_INET;
        struct hostent* addr = gethostbyname(addr_s.c_str());
        bcopy((char*)addr->h_addr,
            (char*)&serv_addr.sin_addr.s_addr,
            addr->h_length);
        //serv_addr.sin_addr.s_addr = INADDR_ANY;
        serv_addr.sin_port = htons(port);
        //attempt to bind to port
        isInitialized = (bind(socketHandle, (struct sockaddr*)(&serv_addr), sizeof(serv_addr))==0);
        if(!isInitialized)
        {
            LOG_ERROR("Error: %s(code:%d)",strerror(errno),errno);
        }
        return isInitialized;
    }

    SocketHandle SocketImpl::acceptNext()
    {
        if(socketHandle < 0 || socketMode != Mode::Server){
            return nullptr;
        }
        struct sockaddr_in cli_addr;
        listen(socketHandle, 1);
        socklen_t clilen = sizeof(cli_addr);
        int32_t acceptedHandle = accept(socketHandle, (struct sockaddr*)&cli_addr, &clilen);
        if (acceptedHandle < 0) {
            return nullptr;
        }
        return SocketHandle(new SocketImpl(acceptedHandle, Mode::Client, true));
    }

    int32_t SocketImpl::available()
    {
        if (socketHandle < 0 || !isInitialized) {
            return 0;
        }
        int count;
        ioctl(socketHandle, FIONREAD, &count);
        return count;
    }

    bool SocketImpl::initFd()
    {
        if (socketHandle != -1) {
            return false;
        }
        //open a socket file descriptor
        socketHandle = socket(AF_INET, SOCK_STREAM, 0);
        if (socketHandle < 0) {
            return false;
        }
        return true;
    }

} // namespace socket
} // namespace commproto
