
#include <ThermostatDevice.h>
#include <SocketImpl.h>
#include <AuthChains.h>
#include <commproto/logger/Logging.h>
#include <commproto/parser/ParserDelegatorUtils.h>




class AcceptHandler : public commproto::parser::Handler
{
public:
	AcceptHandler(Thermostat& thermo_)
		: thermo(thermo_)
	{
	}
	void handle(commproto::messages::MessageBase&& data) override
	{
		ConnectionAuthorizedMessage& msg = static_cast<ConnectionAuthorizedMessage&>(data);
		if (msg.prop.size() != 3)
		{
			LOG_ERROR("Connection authorization message has too few arguments");
			thermo.reject();
		}
		thermo.accept(APData{ msg.prop[0], msg.prop[1],msg.prop[2],msg.prop2 });
	}
private:
	Thermostat& thermo;
};

class RejectHandler : public commproto::parser::Handler
{
public:
	RejectHandler(Thermostat& thermo_)
		: thermo(thermo_)
	{
	}
	void handle(commproto::messages::MessageBase&& data) override
	{
		thermo.reject();
	}
private:
	Thermostat& thermo;
};





commproto::parser::ParserDelegatorHandle build(Thermostat& device)
{
	commproto::parser::ParserDelegatorHandle delegator = std::make_shared<commproto::parser::ParserDelegator>();
	commproto::parser::buildBase(delegator);
	commproto::parser::addParserHandlerPair<ConnectionAuthorizedParser, ConnectionAuthorizedMessage>(delegator, std::make_shared<AcceptHandler>(device));
	commproto::parser::addParserHandlerPair<ConnectionRejectedParser, ConnectionRejectedMessage>(delegator, std::make_shared<RejectHandler>(device));


	return delegator;
}


const APData defaultAP = { "CPEP::Thermostat","COMPROTO","192.168.1.10",9001 };

const DeviceDetails thisDevice = { "Thermostat","Commproto","A simple device that provides data about temperature, humidity and the possibility to start heating." };





Thermostat::Thermostat(ThermostatWrapper& wrapper)
	: device(wrapper)
	, state{ State::CheckAuth }
{

}

void Thermostat::setup()
{
	serial = device.getStream(115200);
}



void Thermostat::loop()
{
	switch (state)
	{
	case State::CheckAuth:

		if (!device.hasAuth())
		{
			socket = device.startAsAP(defaultAP);
			state = State::SendAuthData;
		}
		else
		{
			socket = device.connect(device.getAuthData());
			state = State::StartAsEP;
		}

		return;
	case State::SendAuthData:
	{
		if (!socket)
		{
			return;
		}
		commproto::sockets::SocketHandle client = socket->acceptNext();
		if (!client || !client->connected())
		{
			return;
		}
		mapper = commproto::messages::TypeMapperFactory::build(client);
		delegator = build(*this);
		builder = std::make_shared<commproto::parser::MessageBuilder>(client, delegator);
		uint32_t authMsgId = mapper->registerType<DeviceDataMessage>();
		socket->sendByte(sizeof(void*));
		commproto::Message authMsg = DeviceDataSerializer::serialize(std::move(DeviceDataMessage(authMsgId, thisDevice.name, thisDevice.manufacturer, thisDevice.description)));
		int sent = socket->sendBytes(authMsg);
		if (sent == authMsg.size())
		{
			state = State::WaitForResponse;
			LOG_ERROR("A problem occurred when sending device details");
			return;
		}
		LOG_ERROR("A problem occurred when sending device details");
		return;

	}
	case State::WaitForResponse:
	{
		device.delay(100);
		builder->pollAndReadTimes(10);
		device.delay(100);
	}
	return;
	case State::StartAsEP:
	{

	}
	return;
	}
}

void Thermostat::accept(const APData& data)
{
	device.saveAPData(data);
	LOG_INFO("Got authentification data");
	LOG_INFO("Server SSID:\"%s\" PASS:\"%s\" ADDR:%s:%d",data.ssid,data.password,data.addr,data.port);
	device.reboot();
}

void Thermostat::reject()
{
	state = State::SendAuthData;
	delegator.reset();
	builder.reset();
	mapper.reset();
	LOG_INFO("Got rejected :(");
}
