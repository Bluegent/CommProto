#ifndef THERMO_DEVICE_H
#define THERMO_DEVICE_H

#include <ThermostatDeviceWrapper.h>
#include <commproto/messages/TypeMapper.h>
#include <commproto/parser/MessageBuilder.h>

enum class State
{
	CheckAuth,
	SendAuthData,
	WaitForResponse,
	StartAsEP,
};


struct DeviceDetails
{
	std::string name;
	std::string manufacturer;
	std::string description;
};

class Thermostat
{
public:
	Thermostat(ThermostatWrapper& wrapper);
	void setup();
	void loop();
	void accept(const APData& data);
	void reject();
private:
	ThermostatWrapper& device;
	commproto::stream::StreamHandle serial;
	State state;
	commproto::sockets::SocketHandle socket;
	commproto::sockets::SocketHandle client;
	commproto::messages::TypeMapperHandle mapper;
	commproto::parser::MessageBuilderHandle builder;
	commproto::parser::ParserDelegatorHandle delegator;

};

#endif //THERMO_DEVICE_H