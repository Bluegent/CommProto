#ifndef BASE_ENDPOINT_WRAPPER
#define BASE_ENDPOINT_WRAPPER

#include <stdint.h>
#include <commproto/sockets/Socket.h>
#include <commproto/authdevice/Structures.h>

namespace commproto
{
	namespace endpointdevice
	{
		class BaseEndpointWrapper
		{
		public:
			virtual ~BaseEndpointWrapper() = default;
			virtual stream::StreamHandle getStream(int speed) = 0;
			virtual bool hasAuth() = 0;
			virtual authdevice::ConnectionData getAuthData() = 0;
			virtual sockets::SocketHandle startAsAP(const authdevice::ConnectionData & data) = 0;
			virtual sockets::SocketHandle connect(const authdevice::ConnectionData& data) = 0;
			virtual void saveAPData(const authdevice::ConnectionData& data) = 0;
			virtual void resetAPData() = 0;
			virtual void delayT(uint32_t msec) = 0;
			virtual void reboot() = 0;
			virtual bool readAPData() = 0;
			virtual void initFs() = 0;
		};
	}
}


#endif //BASE_ENDPOINT_WRAPPER