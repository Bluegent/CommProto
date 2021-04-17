#ifndef AUTH_DEVICE_WRAPPER_H
#define AUTH_DEVICE_WRAPPER_H

#include <vector>
#include <commproto/sockets/Socket.h>
#include <commproto/authdevice/Structures.h>
#include <SerialInterface.h>

namespace commproto
{
	namespace authdevice
	{
		class AuthDeviceWrapper
		{
		public:
			virtual ~AuthDeviceWrapper() = default;
			virtual serial::SerialHandle getSerial(const int speed) = 0;
			virtual std::vector<std::string> listNetworks() = 0;
			virtual sockets::SocketHandle connectTo(const ConnectionData & data) = 0;
			virtual void delayT(const uint32_t delay) = 0;
			virtual void setLED(const bool on) = 0;
		};
	}
}

#endif //AUTH_DEVICE_WRAPPER_H