#include <AuthDevice.h>
#include <AuthChains.h>
#include <commproto/parser/MessageBuilder.h>
#include <commproto/messages/TypeMapper.h>
#include <commproto/parser/ParserDelegatorFactory.h>
#include <commproto/parser/ParserDelegatorUtils.h>
#include "commproto/logger/Logging.h"


DeviceDataHandler::DeviceDataHandler(AuthDevice* device_) :
	device{ device_ }
{
}

void DeviceDataHandler::handle(commproto::messages::MessageBase&& data)
{
	if (!device)
	{
		return;
	}
	DeviceDataMessage& msg = static_cast<DeviceDataMessage&>(data);
	EndpointData epData;
	epData.name = msg.prop;
	epData.manufacturer = msg.prop2;
	epData.description = msg.prop3;
	device->finishReading(epData);

}


commproto::parser::ParserDelegatorHandle build(AuthDevice* device)
{
	commproto::parser::ParserDelegatorHandle delegator = std::make_shared<commproto::parser::ParserDelegator>();
	commproto::parser::buildBase(delegator);
	commproto::parser::addParserHandlerPair<DeviceDataParser, DeviceDataMessage>(delegator, std::make_shared<DeviceDataHandler>(device));

	return delegator;
}


AuthDevice::AuthDevice(AuthDeviceWrapper* device_)
	: device{ device_ }
	, finishedReading(false)
{
}

void AuthDevice::setup()
{
	LOG_INFO("Setup called");
	device->setBaudRate(115200);
}

void AuthDevice::loop()
{
	LOG_INFO("Loop function");
	if(!device)
	{
		return;
	}
	std::vector<std::string> networks = device->listNetworks();
	for (auto name : networks)
	{
		// list wifi networks
		// connect to ones that start with "CPEP" - password is "COMPROTO"
		// connect to 192.168.1.10 
		// read name, manufacturer, description of device
		// send device a message containing rpi SSID, password, dispatch server ip and port


		if (name.find("CPEP") == 0)
		{
			finishedReading = false;
			targetDevice.reset();
			commproto::sockets::SocketHandle connection = device->connectTo(name, "COMPROTO", "192.168.1.10", 9001);
			if(!connection)
			{
				LOG_INFO("Could not connect to network %s", name.c_str());
				return;
			}
			commproto::parser::ParserDelegatorHandle delegator = build(this);
			commproto::parser::MessageBuilderHandle builder = std::make_shared<commproto::parser::MessageBuilder>(connection, delegator);
			commproto::messages::TypeMapperHandle mapper = commproto::messages::TypeMapperFactory::build(connection);
			uint32_t authMsg = mapper->registerType<ConnectionAuthorizedMessage>();
			uint32_t rejectedMsg = mapper->registerType<ConnectionRejectedMessage>();


			while (!finishedReading)
			{
				builder->pollAndRead();
			}
			std::vector<std::string> authParams;
			authParams.push_back("CP::Hub");
			authParams.push_back("commprotopassword");
			authParams.push_back("192.168.1.8");
			connection->sendBytes(ConnectionAuthorizedSerializer::serialize(std::move(ConnectionAuthorizedMessage(authMsg, authParams, 25565))));
		}
	}

}

void AuthDevice::finishReading(const EndpointData& data)
{
	finishedReading = true;
	targetDevice = data;
}
