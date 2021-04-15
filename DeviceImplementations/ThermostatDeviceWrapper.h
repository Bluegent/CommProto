#ifndef THERMO_DEVICE_WRAPPER_H
#define THERMO_DEVICE_WRAPPER_H

#include <stdint.h>
#include <commproto/sockets/Socket.h>


struct APData
{
	std::string ssid;
	std::string password;
	std::string addr;
	uint32_t port;
};

class BaseEndpointWrapper
{
public:
	virtual ~BaseEndpointWrapper() = default;
	virtual commproto::stream::StreamHandle getStream(int speed) = 0;
	virtual bool hasAuth() = 0;
	virtual APData getAuthData() = 0;
	virtual commproto::sockets::SocketHandle startAsAP(const APData & data) = 0;
	virtual commproto::sockets::SocketHandle connect(const APData& data) = 0;
	virtual void saveAPData(const APData& data) = 0;
	virtual void delayT(uint32_t msec) = 0;
	virtual void reboot() = 0;
};

class ThermostatWrapper : public BaseEndpointWrapper
{
public:
	virtual int32_t getTemp() = 0;
};

#endif //THERMO_DEVICE_WRAPPER_H