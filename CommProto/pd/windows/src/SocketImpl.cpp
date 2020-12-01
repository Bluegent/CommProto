#include "../interface/SocketImpl.h"
#include <sstream>
#include <windows.h>
#include <ws2tcpip.h>
#include <Logging.h>

namespace commproto {
	namespace sockets {

		bool SocketImpl::isWsaStarted = false;

		SocketImpl::SocketImpl()
			: socketMode(Mode::Unassigned)
			, isInitialized(false)
			, socketHandle(INVALID_SOCKET)

		{
			startWsa();
		}

		SocketImpl::~SocketImpl()
		{
			closesocket(socketHandle);
			socketHandle = INVALID_SOCKET;
		}

		SocketImpl::SocketImpl(SOCKET handle, const Mode mode, const bool initialized)
			: socketMode(mode)
			, isInitialized(initialized)
			, socketHandle(handle)
		{
		}

		uint32_t SocketImpl::sendBytes(const Message& message)
		{
			if (!isWsaStarted || !isInitialized)
			{
				return 0;
			}
			LOG_INFO("Sent %ld bytes...",static_cast<uint32_t>(message.size()));
			return send(socketHandle, message.data(), message.size(), 0);
		}

		uint32_t SocketImpl::receive(Message& message, const uint32_t size)
		{
			if (!isWsaStarted || !isInitialized)
			{
				return 0;
			}
			message.reserve(size);
			LOG_INFO("Read %ld bytes...", static_cast<uint32_t>(message.size()));
			return recv(socketHandle, message.data(), size, 0);
		}

		char SocketImpl::readByte()
		{
			if (!isWsaStarted || !isInitialized)
			{
				return 0;
			}
			char output=0;
			recv(socketHandle, &output,1,0);
			return output;
		}

		bool SocketImpl::initClient(const std::string& addr, uint32_t port)
		{
			if (socketMode != Mode::Unassigned || !isWsaStarted)
			{
				return false;
			}
			socketMode = Mode::Client;

			struct addrinfo *result = NULL,
				hints;
			ZeroMemory(&hints, sizeof(hints));
			hints.ai_family = AF_UNSPEC;
			hints.ai_socktype = SOCK_STREAM;
			hints.ai_protocol = IPPROTO_TCP;
			std::stringstream stream;
			stream << port;

			//resolve server address
			int iResult = getaddrinfo(addr.c_str(), stream.str().c_str(), &hints, &result);
			if (iResult != 0) {
				return false;
			}
			
			struct addrinfo outgoingAddr = result[0];
			//initialize socket handle
			socketHandle = socket(outgoingAddr.ai_family, outgoingAddr.ai_socktype, outgoingAddr.ai_protocol);
			if (socketHandle == INVALID_SOCKET) {
				return false;
			}

			// attempt to connect to server
			iResult = connect(socketHandle, outgoingAddr.ai_addr, (int)outgoingAddr.ai_addrlen);
			if (iResult == SOCKET_ERROR) {
				closesocket(socketHandle);
				socketHandle = INVALID_SOCKET;
				return  false;
			}

			isInitialized = true;
			freeaddrinfo(result);
			return true;
		}

		bool SocketImpl::initServer(uint32_t port)
		{
			if (socketMode != Mode::Unassigned || !isWsaStarted)
			{
				return false;
			}
			socketMode = Mode::Server;

			struct addrinfo hints;
			struct addrinfo *result;
			ZeroMemory(&hints, sizeof(hints));
			hints.ai_family = AF_INET;
			hints.ai_socktype = SOCK_STREAM;
			hints.ai_protocol = IPPROTO_TCP;
			hints.ai_flags = AI_PASSIVE;
			std::stringstream stream;
			stream << port;

			// resolve our own address
			int iResult = getaddrinfo(NULL, stream.str().c_str(), &hints, &result);
			if (iResult != 0) {
				return false;
			}

			//open a server socket
			socketHandle = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
			if (socketHandle == INVALID_SOCKET) {
				closesocket(socketHandle);
				socketHandle = INVALID_SOCKET;
				return false;
			}

			//bind to port
			iResult = bind(socketHandle, result->ai_addr, (int)result->ai_addrlen);
			if (iResult == SOCKET_ERROR) {
				return false;
			}

			isInitialized = true;
			freeaddrinfo(result);
			return true;
		}

		SocketHandle SocketImpl::acceptNext()
		{
			if (socketMode != Mode::Server || !isInitialized)
			{
				return false;
			}
			//listen for a connection
			int iResult = listen(socketHandle, 1);
			if (iResult == SOCKET_ERROR) {
				int err = WSAGetLastError();
				return nullptr;
			}

			// accept a connection
			SOCKET newConnection = accept(socketHandle, NULL, NULL);
			if (newConnection == INVALID_SOCKET) {
				return nullptr;
			}

			return SocketHandle(new SocketImpl(newConnection, Mode::Client, true));
		}

		uint32_t SocketImpl::pollSocket()
		{
			if (socketHandle < 0 || !isInitialized) {
				return 0;
			}
			unsigned long count,out;
			WSAIoctl(socketHandle, FIONREAD,nullptr, 0, &count, sizeof(count),&out,nullptr,nullptr);
			return static_cast<uint32_t>(count);
		}

		WsaStartupResult SocketImpl::startWsa()
		{
			if (isWsaStarted)
			{
				return WsaStartupResult::AlreadyStarted;
			}

			WSADATA wsaData;
			int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
			if (iResult != 0)
			{
				printf("WSAStartup failed with error: %d\n", iResult);
				return WsaStartupResult::StartupFailed;
			}
			isWsaStarted = true;
			return WsaStartupResult::StartupSucceeded;
		}

		void SocketImpl::stopWsa()
		{
			WSACleanup();
		}
	} // namespace socket
} // namespace commproto
