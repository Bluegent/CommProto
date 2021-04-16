#ifndef AUTH_DEVICE_WRAPPER_H
#define AUTH_DEVICE_WRAPPER_H

#include <vector>
#include <commproto/sockets/Socket.h>
#include <SerialInterface.h>

class AuthDeviceWrapper
{
public:
	virtual ~AuthDeviceWrapper() = default;
	virtual commproto::serial::SerialHandle getSerial(const int speed) = 0;
	virtual std::vector<std::string> listNetworks() = 0;
	virtual commproto::sockets::SocketHandle connectTo(const std::string & ssid, const std::string & pwd, const std::string& addr, const uint32_t port) = 0;
	virtual void delayT(const uint32_t delay) = 0;
	virtual void setLED(const bool on) = 0;
};

#endif //AUTH_DEVICE_WRAPPER_H