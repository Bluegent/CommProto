
#include <ThermostatDevice.h>
#include <SocketImpl.h>
#include <commproto/device/AuthChains.h>
#include <commproto/logger/Logging.h>
#include <commproto/parser/ParserDelegatorUtils.h>




class AcceptHandler : public commproto::parser::Handler
{
public:
	AcceptHandler(BaseEndpointAuth& thermo_)
		: thermo(thermo_)
	{
	}
	void handle(commproto::messages::MessageBase&& data) override
	{
		commproto::device::ConnectionAuthorizedMessage& msg = static_cast<commproto::device::ConnectionAuthorizedMessage&>(data);
		if (msg.prop.size() != 3)
		{
			LOG_ERROR("Connection authorization message has too few arguments");
			thermo.reject();
		}
		thermo.accept(APData{ msg.prop[0], msg.prop[1],msg.prop[2],msg.prop2 });
	}
private:
	BaseEndpointAuth& thermo;
};

class RejectHandler : public commproto::parser::Handler
{
public:
	RejectHandler(BaseEndpointAuth& thermo_)
		: thermo(thermo_)
	{
	}
	void handle(commproto::messages::MessageBase&& data) override
	{
		thermo.reject();
	}
private:
	BaseEndpointAuth& thermo;
};


commproto::parser::ParserDelegatorHandle build(BaseEndpointAuth& device)
{
	commproto::parser::ParserDelegatorHandle delegator = std::make_shared<commproto::parser::ParserDelegator>();
	commproto::parser::buildBase(delegator);
	commproto::parser::addParserHandlerPair<commproto::device::ConnectionAuthorizedParser, commproto::device::ConnectionAuthorizedMessage>(delegator, std::make_shared<AcceptHandler>(device));
	commproto::parser::addParserHandlerPair<commproto::device::ConnectionRejectedParser, commproto::device::ConnectionRejectedMessage>(delegator, std::make_shared<RejectHandler>(device));


	return delegator;
}


const APData defaultAP = { "CPEP::Thermostat","COMPROTO","192.168.1.10",9001 };

const DeviceDetails thisDevice = { "Thermostat","Commproto","A simple device that provides data about temperature, humidity and the possibility to start heating." };





BaseEndpointAuth::BaseEndpointAuth(BaseEndpointWrapper& wrapper)
	: device(wrapper)
	, state{ BaseAuthState::SendAuthData }
	, responseAttempts{ 0 }
{

}

void BaseEndpointAuth::setup()
{
	serial = device.getStream(115200);
}



void BaseEndpointAuth::loop()
{
	switch (state)
	{
	case BaseAuthState::SendAuthData:
	{
		if (!socket)
		{
			
			socket = device.startAsAP(defaultAP);
			LOG_INFO("Waiting for client");
		}		
		commproto::sockets::SocketHandle client = socket->acceptNext();
		if (!client || !client->connected())
		{
			return;
		}
		LOG_INFO("Sending pointer size...");
		int sent = 0;
		do {
			sent = client->sendByte(sizeof(void*));
			device.delayT(100);
		} while (sent == 0);
		LOG_INFO("Sending pointer size done");

		LOG_INFO("Connected to client, sending authetification data.");
		mapper = commproto::messages::TypeMapperFactory::build(client);
		delegator = build(*this);

		uint32_t authMsgId = mapper->registerType<commproto::device::DeviceDataMessage>();
		commproto::Message authMsg = commproto::device::DeviceDataSerializer::serialize(std::move(commproto::device::DeviceDataMessage(authMsgId, thisDevice.name, thisDevice.manufacturer, thisDevice.description)));
		responseAttempts = 0;
		sent = client->sendBytes(authMsg);
		if (sent == authMsg.size())
		{
			state = BaseAuthState::WaitForReconnect;
			responseAttempts = 0;
			LOG_INFO("Sent auth details, waiting for reply");
			client->shutdown();
			return;
		}
		LOG_ERROR("A problem occurred when sending device details (sent:%d of %d)", sent, authMsg.size());
		return;

	}
	case BaseAuthState::WaitForReconnect:
	{
		commproto::sockets::SocketHandle reconnect = socket->acceptNext();
		if (responseAttempts % 100 == 0) {
			LOG_INFO("Attempt #%d waiting for auth connection", responseAttempts/100);
		}
		if (responseAttempts >= 600)
		{
			LOG_WARNING("Timed out while waiting for a connection");
			state = BaseAuthState::SendAuthData;
		}
		++responseAttempts;
		device.delayT(10);
		
		if (!reconnect || !reconnect->connected())
		{
			return;
		}

		LOG_INFO("Sending pointer size...");
		int sent = 0;
		do {
			sent = reconnect->sendByte(sizeof(void*));
			device.delayT(100);
		} while (sent == 0);
		LOG_INFO("Sending pointer size done");

		delegator = build(*this);
		builder = std::make_shared<commproto::parser::MessageBuilder>(reconnect, delegator);

		responseAttempts = 0;
		LOG_INFO("Waiting for authetification response");

		while(responseAttempts <=10)
		{
			LOG_INFO("Attempt #%d to read", responseAttempts);
			builder->pollAndReadTimes(100);
			device.delayT(6000);
			++responseAttempts;
		}
		LOG_WARNING("Timed out while waiting for a response");
		state = BaseAuthState::SendAuthData;
		reconnect->shutdown();
		return;
	}
	default:;
	}
}

void BaseEndpointAuth::accept(const APData& data)
{
	device.saveAPData(data);
	LOG_INFO("Got authentification data");
	LOG_INFO("Server SSID:\"%s\" PASS:\"%s\" ADDR:%s:%d", data.ssid.c_str(), data.password.c_str(), data.addr.c_str(), data.port);
	device.reboot();
}

void BaseEndpointAuth::reject()
{
	state = BaseAuthState::SendAuthData;
	delegator.reset();
	builder.reset();
	mapper.reset();
	LOG_INFO("Got rejected :(");
}
