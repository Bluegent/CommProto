#include "../interface/SocketImpl.h"
#include <sstream>
#include <windows.h>
#include <ws2tcpip.h>
#include <commproto/logger/Logging.h>

namespace commproto {
	namespace sockets {

		bool SocketImpl::isWsaStarted = false;

		SocketImpl::SocketImpl()
			: socketMode(Mode::Unassigned)
			, isInitialized(false)
			, socketHandle(INVALID_SOCKET)
			, isConnected(false)

		{
			startWsa();
		}

		SocketImpl::~SocketImpl()
		{
			closesocket(socketHandle);
			socketHandle = INVALID_SOCKET;
		}

                void SocketImpl::shutdown()
		{
			closesocket(socketHandle);
		}

		void SocketImpl::setTimeout(const uint32_t msec)
		{
			DWORD timeOut = msec;
			int res = setsockopt(socketHandle, SOL_SOCKET, SO_SNDTIMEO, reinterpret_cast<char*>(&timeOut), sizeof timeOut);
			res = setsockopt(socketHandle, SOL_SOCKET, SO_RCVTIMEO, reinterpret_cast<char*>(&timeOut), sizeof timeOut);
		}

		SocketImpl::SocketImpl(SOCKET handle, const Mode mode, const bool initialized)
			: socketMode(mode)
			, isInitialized(initialized)
			, socketHandle(handle)
			, isConnected(true)
		{
		}

		int32_t SocketImpl::sendBytes(const Message& message)
		{
			if (!isWsaStarted || !isInitialized)
			{
				return 0;
			}			
			int sent = send(socketHandle, message.data(), message.size(), 0);
			if (sent != message.size())
			{
				isConnected = false;
			}

			return sent;
		}

		int32_t SocketImpl::receive(Message& message, const uint32_t size)
		{
			if (!isWsaStarted || !isInitialized)
			{
				return 0;
			}
			message.resize(size);		
			int received = recv(socketHandle, message.data(), size, 0);
			if (received != size)
			{
				isConnected = false;
			}
			return received;
		}

		int SocketImpl::readByte()
		{
			if (!isWsaStarted || !isInitialized)
			{
				return -1;
			}
			char output = 0;
			int received = recv(socketHandle, &output, 1, 0);
			if(received == 0)
			{
				isConnected = false;
			}
			return output;
		}

		int SocketImpl::sendByte(const char byte) 
		{
			if (!isWsaStarted || !isInitialized)
			{
				return 0;
			}
			int sent = send(socketHandle, &byte, 1, 0);
			if (sent != 1)
			{
				isConnected = false;
			}
			return sent;

		}

		bool SocketImpl::connected()
		{
			return isConnected;
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
			isConnected = true;
			return true;
		}

		bool SocketImpl::initServer(const std::string& addr, uint32_t port)
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
			int iResult;

			if(addr.compare("127.0.0.1")==0 || addr.compare("localhost")==0)
			{
				iResult = getaddrinfo(NULL, stream.str().c_str(), &hints, &result);
			}
			else
			{
				iResult = getaddrinfo(addr.c_str(), stream.str().c_str(), &hints, &result);
			}
			// resolve our own address
			
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
				return nullptr;
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

		int32_t SocketImpl::available()
		{
			if (socketHandle < 0 || !isInitialized) {
				return 0;
			}
			unsigned long count, out;
			WSAIoctl(socketHandle, FIONREAD, nullptr, 0, &count, sizeof(count), &out, nullptr, nullptr);
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
