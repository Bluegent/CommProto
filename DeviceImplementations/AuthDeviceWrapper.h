#ifndef DEVICE_WRAPPER_H
#define DEVICE_WRAPPER_H
#include <vector>
#include <commproto/sockets/Socket.h>

class AuthDeviceWrapper
{
public:
	virtual ~AuthDeviceWrapper() = default;

	virtual std::vector<std::string> listNetworks() = 0;
	virtual void setBaudRate(const uint32_t bytes) = 0;
	virtual commproto::sockets::SocketHandle connectTo(const std::string & ssid, const std::string & pwd, const std::string& addr, const uint32_t port) = 0;
};

#endif //DEVICE_WRAPPER_H