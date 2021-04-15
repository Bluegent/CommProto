#include <AuthDevice.h>
#include <commproto/device/AuthChains.h>
#include <commproto/parser/MessageBuilder.h>
#include <commproto/messages/TypeMapper.h>
#include <commproto/parser/ParserDelegatorFactory.h>
#include <commproto/parser/ParserDelegatorUtils.h>
#include <commproto/logger/Logging.h>
#include <commproto/parser/Handler.h>
#include <commproto/logger/LoggingMessage.h>
#include <algorithm>


class DeviceDataHandler : public commproto::parser::Handler
{
public:
	DeviceDataHandler(AuthDevice& device_, const std::string & name_);
	void handle(commproto::messages::MessageBase&& data) override;
private:
	AuthDevice& device;
	std::string name;
};

DeviceDataHandler::DeviceDataHandler(AuthDevice& device_, const std::string & name_)
	: device(device_)
	, name{ name_ }
{
}

void DeviceDataHandler::handle(commproto::messages::MessageBase&& data)
{
	commproto::device::DeviceDataMessage& msg = static_cast<commproto::device::DeviceDataMessage&>(data);
	EndpointData epData;
	epData.name = msg.prop;
	epData.manufacturer = msg.prop2;
	epData.description = msg.prop3;
	device.finishReading(epData, name);

}


class DeviceRejectandler : public commproto::parser::Handler
{
public:
	DeviceRejectandler(AuthDevice& device_);
	void handle(commproto::messages::MessageBase&& data) override;
private:
	AuthDevice& device;
};

DeviceRejectandler::DeviceRejectandler(AuthDevice& device_)
	: device(device_)
{
}

void DeviceRejectandler::handle(commproto::messages::MessageBase&& data)
{
	commproto::device::DeviceAuthReject& msg = static_cast<commproto::device::DeviceAuthReject&>(data);
	LOG_INFO("Denied %s", msg.prop.c_str());
	device.responseDeny(msg.prop);
}

class DeviceAuthHandler : public commproto::parser::Handler
{
public:
	DeviceAuthHandler(AuthDevice& device_);
	void handle(commproto::messages::MessageBase&& data) override;
private:
	AuthDevice& device;
};

DeviceAuthHandler::DeviceAuthHandler(AuthDevice& device_)
	: device(device_)
{
}

void DeviceAuthHandler::handle(commproto::messages::MessageBase&& data)
{
	commproto::device::DeviceAuthAccept& msg = static_cast<commproto::device::DeviceAuthAccept&>(data);
	LOG_INFO("Accepted %s", msg.prop.c_str());
	device.responseAccept(msg.prop, msg.prop2, msg.prop3);
}


class ScanHandler : public commproto::parser::Handler
{
public:
	ScanHandler(AuthDevice& device_)
		: device(device_)
	{
	}

	void handle(commproto::messages::MessageBase&& data) override
	{
		device.scan();
	}
private:
	AuthDevice& device;
};





commproto::parser::ParserDelegatorHandle buildSerialDelegator(AuthDevice& device)
{
	commproto::parser::ParserDelegatorHandle delegator = std::make_shared<commproto::parser::ParserDelegator>();
	commproto::parser::buildBase(delegator);
	commproto::parser::addParserHandlerPair<commproto::device::ScanForNetworksParser, commproto::device::ScanForNetworksMessage>(delegator, std::make_shared<ScanHandler>(device));
	commproto::parser::addParserHandlerPair<commproto::device::DeviceAuthAcceptParser, commproto::device::DeviceAuthAccept>(delegator, std::make_shared<DeviceAuthHandler>(device));
	commproto::parser::addParserHandlerPair<commproto::device::DeviceAuthRejectParser, commproto::device::DeviceAuthReject>(delegator, std::make_shared<DeviceRejectandler>(device));

	return delegator;
}


commproto::parser::ParserDelegatorHandle buildDeviceDelegator(AuthDevice& device, const std::string& name)
{
	commproto::parser::ParserDelegatorHandle delegator = std::make_shared<commproto::parser::ParserDelegator>();
	commproto::parser::buildBase(delegator);
	commproto::parser::addParserHandlerPair<commproto::device::DeviceDataParser, commproto::device::DeviceDataMessage>(delegator, std::make_shared<DeviceDataHandler>(device, name));


	return delegator;
}


LogToSerial::LogToSerial(const commproto::serial::SerialHandle& serial, const uint32_t logMsgId)
	: serial(serial)
	, logMsgId(logMsgId)
{
}

void LogToSerial::addLog(const char* line, const uint32_t size)
{
	if (!serial)
	{
		return;
	}
	std::string message(line, size);
	commproto::Message log = commproto::logger::LogSerializer::serialize(std::move(commproto::logger::LogMessage(logMsgId, "AuthDevice", message)));
	serial->sendBytes(log);
}

AuthDevice::AuthDevice(AuthDeviceWrapper& device_)
	: device(device_)
	, finishedReading(false)
	, serviceConnected(false)
	, shouldScan(false)
{
}

void AuthDevice::setup()
{
	serial = device.getSerial(115200);
	device.setLED(false);
}

void AuthDevice::loop()
{

	if (!serviceConnected)
	{
		if (serial && serial->available() > 0)
		{
			serial->sendByte(sizeof(void*));


			device.setLED(true);

			mapper = commproto::messages::TypeMapperFactory::build(serial);
			log = std::make_shared<LogToSerial>(serial, mapper->registerType<commproto::logger::LogMessage>());
			commproto::logger::setLoggable(log.get());

			provider = std::make_shared<IdProvider>(mapper);
			builder = std::make_shared<commproto::parser::MessageBuilder>(serial, buildSerialDelegator(*this));

			LOG_INFO("Service connection established");
			serviceConnected = true;

			return;
		}
		return;
	}


	if (shouldScan)
	{
		scanNetworks();
		shouldScan = false;
	}

	if (builder)
	{
		builder->pollAndReadTimes(100);
	}

	device.delayT(1000);
}

void AuthDevice::finishReading(const EndpointData& data, const std::string & name)
{
	finishedReading = true;
	targetDevice = data;
	std::vector<std::string> props;
	props.push_back(name);
	props.push_back(data.name);
	props.push_back(data.manufacturer);
	props.push_back(data.description);
	commproto::Message authMsg = commproto::device::DeviceAuthRequestSerializer::serialize(std::move(commproto::device::DeviceAuthRequestMessage(
		provider->deviceAuthId,
		props)));

	serial->sendBytes(authMsg);
}

void AuthDevice::scan()
{
	shouldScan = true;
}


void sendMessage(const commproto::sockets::SocketHandle & socket, const commproto::Message& message)
{

	int sent = socket->sendBytes(message);
	if (sent != message.size())
	{
		LOG_ERROR("Incomplete message (%d of %d)", sent, message.size());
	}

}

void AuthDevice::responseAccept(const std::string & name, const std::vector<std::string> & reply, const uint32_t port)
{
	LOG_INFO("Authorized device \"%s\", sending message", name.c_str());
	if (reply.size() != 3)
	{
		LOG_ERROR("Incomplete reply for connection %s", name.c_str());
		return;
	}

	commproto::sockets::SocketHandle connection = device.connectTo(name, "COMPROTO", "192.168.1.10", 9001);
	if (!connection)
	{
		LOG_INFO("Could not connect to network \"%s\"", name.c_str());
		return;
	}

	uint32_t attempts = 0;
	do
	{
		++attempts;
		device.delayT(100);
	} while (attempts < 5 && !connection->connected());


	LOG_INFO("Socket open and connected(? %s) \"%s\"", (connection->connected() ? "true" : "false"), name.c_str());

	connection->sendByte(sizeof(void*));
	commproto::messages::TypeMapperHandle connectionMapper = commproto::messages::TypeMapperFactory::build(connection);
	uint32_t id = connectionMapper->registerType<commproto::device::ConnectionAuthorizedMessage>();

	commproto::Message msg = commproto::device::ConnectionAuthorizedSerializer::serialize(commproto::device::ConnectionAuthorizedMessage(id, reply, port));

	sendMessage(connection, msg);
	auto it = std::find(previouslyScanned.begin(), previouslyScanned.end(), name);
	previouslyScanned.erase(it);
	LOG_INFO("Sent approval message to \"%s\"", name.c_str());

}

void AuthDevice::responseDeny(const std::string& name)
{
	LOG_INFO("Rejected device \"%s\", sending message", name.c_str());
	commproto::sockets::SocketHandle connection = device.connectTo(name, "COMPROTO", "192.168.1.10", 9001);
	if (!connection)
	{
		LOG_INFO("Could not connect to network \"%s\"", name.c_str());
		return;
	}

	uint32_t attempts = 0;
	do
	{
		++attempts;
		device.delayT(100);
	} while (attempts < 5 && !connection->connected());

	connection->sendByte(sizeof(void*));
	commproto::messages::TypeMapperHandle connectionMapper = commproto::messages::TypeMapperFactory::build(connection);
	uint32_t id = connectionMapper->registerType<commproto::device::ConnectionRejectedMessage>();

	commproto::Message  msg = commproto::device::ConnectionRejectedSerializer::serialize(commproto::device::ConnectionRejectedMessage(id));
	sendMessage(connection, msg);
}



void AuthDevice::scanNetworks()
{
	LOG_INFO("Scanning networks");
	std::vector<std::string> networks = device.listNetworks();
	for (auto name : networks)
	{
		// list wifi networks
		// connect to ones that start with "CPEP" - password is "COMPROTO"
		// connect to 192.168.1.10 
		// read name, manufacturer, description of device
		// send device a message containing rpi SSID, password, dispatch server ip and port


		if (!alreadyScanned(name) && name.find("CPEP::") == 0)
		{
			finishedReading = false;
			targetDevice.reset();
			commproto::sockets::SocketHandle connection = device.connectTo(name, "COMPROTO", "192.168.1.10", 9001);
			if (!connection)
			{
				LOG_WARNING("Could not connect to network %s", name.c_str());
				return;
			}
			connection->sendByte(sizeof(void*));

			commproto::parser::ParserDelegatorHandle delegator = buildDeviceDelegator(*this, name);
			commproto::parser::MessageBuilderHandle builder = std::make_shared<commproto::parser::MessageBuilder>(connection, delegator);

			LOG_INFO("Waiting for device details...");
			while (!finishedReading)
			{
				builder->pollAndReadTimes(100);
				device.delayT(1);
			}
			LOG_INFO("Device details received, closing connection");
			previouslyScanned.push_back(name);
			connection->shutdown();
		}
	}
	serial->sendBytes(commproto::device::ScanFinishedSerializer::serialize(std::move(commproto::device::ScanFinished(provider->finishScanId))));
}

bool AuthDevice::alreadyScanned(const std::string& name)
{
	return  std::find(previouslyScanned.begin(), previouslyScanned.end(), name) != previouslyScanned.end();
}
