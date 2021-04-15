#ifndef THERMO_DEVICE_H
#define THERMO_DEVICE_H

#include <ThermostatDeviceWrapper.h>
#include <commproto/messages/TypeMapper.h>
#include <commproto/parser/MessageBuilder.h>

enum class BaseAuthState
{
	SendAuthData,
	WaitForReconnect,
	ReadForResponse,
};


struct DeviceDetails
{
	std::string name;
	std::string manufacturer;
	std::string description;
};



class BaseEndpointAuth
{
public:
	BaseEndpointAuth(BaseEndpointWrapper& wrapper);
	void setup();
	void loop();
	void accept(const APData& data);
	void reject();
private:
	BaseEndpointWrapper& device;
	commproto::stream::StreamHandle serial;
	BaseAuthState state;
	commproto::sockets::SocketHandle socket;
	commproto::messages::TypeMapperHandle mapper;
	commproto::parser::MessageBuilderHandle builder;
	commproto::parser::ParserDelegatorHandle delegator;
	uint32_t responseAttempts;

};

#endif //THERMO_DEVICE_H