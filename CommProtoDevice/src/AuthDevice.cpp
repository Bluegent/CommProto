#include <commproto/authdevice/AuthDevice.h>
#include <commproto/device/AuthChains.h>
#include <commproto/parser/MessageBuilder.h>
#include <commproto/messages/TypeMapper.h>
#include <commproto/parser/ParserDelegatorFactory.h>
#include <commproto/parser/ParserDelegatorUtils.h>
#include <commproto/logger/Logging.h>
#include <commproto/authdevice/AuthHandlers.h>
#include <algorithm>
#include <commproto/logger/LoggingMessage.h>
#include <commproto/messages/KeepAlive.h>


namespace commproto
{
	namespace authdevice
	{
		parser::ParserDelegatorHandle buildSerialDelegator(AuthDevice& device)
		{
			parser::ParserDelegatorHandle delegator = std::make_shared<parser::ParserDelegator>();
			parser::DelegatorUtils::buildBase(delegator);
			parser::DelegatorUtils::addParserHandlerPair<device::ScanForNetworksParser, device::ScanForNetworksMessage>(delegator, std::make_shared<ScanHandler>(device));
			parser::DelegatorUtils::addParserHandlerPair<device::DeviceAuthAcceptParser, device::DeviceAuthAccept>(delegator, std::make_shared<DeviceAuthHandler>(device));
			parser::DelegatorUtils::addParserHandlerPair<device::DeviceAuthRejectParser, device::DeviceAuthReject>(delegator, std::make_shared<DeviceRejectandler>(device));
			parser::DelegatorUtils::addParserHandlerPair<device::KeepAliveParser, device::KeepAlive>(delegator, std::make_shared<KeepAliveHandler>(device));

			return delegator;
		}


		parser::ParserDelegatorHandle buildDeviceDelegator(AuthDevice& device, const std::string& name)
		{
			parser::ParserDelegatorHandle delegator = std::make_shared<parser::ParserDelegator>();
			parser::DelegatorUtils::buildBase(delegator);
			parser::DelegatorUtils::addParserHandlerPair<device::DeviceDataParser, device::DeviceDataMessage>(delegator, std::make_shared<DeviceDataHandler>(device, name));


			return delegator;
		}


		AuthDevice::AuthDevice(AuthDeviceWrapper& device_)
			: device(device_)
			, finishedReading(false)
			, serviceConnected(false)
			, shouldScan(false)
			, lastPing(0)
			, lastPong(0)
			, lastCheck(device.getMs())
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

					mapper = messages::TypeMapperFactory::build(serial);
					log = std::make_shared<serial::LogToSerial>(serial, mapper->registerType<logger::LogMessage>());
					logger::setLoggable(log.get());

					provider = std::make_shared<IdProvider>(mapper);
					builder = std::make_shared<parser::MessageBuilder>(serial, buildSerialDelegator(*this));


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


			uint32_t now = device.getMs();
			if (now - lastCheck > 5000) {
				lastCheck = now;
				int64_t ping = lastPing;
				int64_t pong = lastPong;
				if ( abs(ping-pong)> 5000)
				{
					LOG_DEBUG("Rebooting because service timed out %d %d", lastPing, lastPong);
					device.reboot();
				}
			}

			sendPing();
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
			Message authMsg = device::DeviceAuthRequestSerializer::serialize(std::move(device::DeviceAuthRequestMessage(
				provider->deviceAuthId,
				props)));

			serial->sendBytes(authMsg);
		}

		void AuthDevice::scan()
		{
			shouldScan = true;
		}


		void sendMessage(const sockets::SocketHandle & socket, const Message& message)
		{

			int sent = socket->sendBytes(message);
			if (sent != message.size())
			{
				LOG_ERROR("Incomplete message (%d of %d)", sent, message.size());
			}

		}

		void AuthDevice::responseAccept(const std::string & name, const std::vector<std::string> & reply, const uint32_t port)
		{
			disableKeepAlive();
			LOG_INFO("Authorized device \"%s\", sending message", name.c_str());
			if (reply.size() != 3)
			{
				LOG_ERROR("Incomplete reply for connection %s", name.c_str());
				return;
			}
			ConnectionData data = ConnectionData::defaultData;
			data.ssid = name;
			sockets::SocketHandle connection = device.connectTo(data);
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
			device.delayT(100);
			connection->sendByte(sizeof(void*));
			messages::TypeMapperHandle connectionMapper = messages::TypeMapperFactory::build(connection);
			uint32_t id = connectionMapper->registerType<device::ConnectionAuthorizedMessage>();

			Message msg = device::ConnectionAuthorizedSerializer::serialize(device::ConnectionAuthorizedMessage(id, reply, port));

			sendMessage(connection, msg);
			auto it = std::find(previouslyScanned.begin(), previouslyScanned.end(), name);
			previouslyScanned.erase(it);
			LOG_INFO("Sent approval message to \"%s\"", name.c_str());
			device.delayT(500);
			connection->shutdown();
			enableKeepAlive();
		}

		void AuthDevice::responseDeny(const std::string& name)
		{
			LOG_INFO("Rejected device \"%s\", sending message", name.c_str());
			ConnectionData data = ConnectionData::defaultData;
			data.ssid = name;
			sockets::SocketHandle connection = device.connectTo(data);
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
			device.delayT(100);
			connection->sendByte(sizeof(void*));
			messages::TypeMapperHandle connectionMapper = messages::TypeMapperFactory::build(connection);
			uint32_t id = connectionMapper->registerType<device::ConnectionRejectedMessage>();

			Message  msg = device::ConnectionRejectedSerializer::serialize(device::ConnectionRejectedMessage(id));
			sendMessage(connection, msg);
			device.delayT(500);
			connection->shutdown();
		}

		void AuthDevice::reboot()
		{
			device.reboot();
		}

		void AuthDevice::sendPing()
		{
			if (!serial)
			{
				return;
			}
			Message keepAlive = device::KeepAliveSerializer::serialize(messages::KeepAliveMessage(provider->keepAliveId));
			serial->sendBytes(keepAlive);
			lastPing = device.getMs();
		}

		void AuthDevice::gotPong()
		{
			lastPong = device.getMs();
		}

		void AuthDevice::disableKeepAlive()
		{
			LOG_DEBUG("Disabling keep alive");
			lastPing = 0;
			lastPong = 0;
			keepAliveOn = false;
		}

		void AuthDevice::enableKeepAlive()
		{
			LOG_DEBUG("Enabling keep alive");
			lastPing = 0;
			lastPong = 0;
			keepAliveOn = true;
		}

		// list wifi networks
		// connect to ones that start with "CPEP" - password is "COMPROTO"
		// connect to 192.168.1.10 
		// read name, manufacturer, description of device
		// send device a message containing rpi SSID, password, dispatch server ip and port
		void AuthDevice::scanNetworks()
		{
			LOG_INFO("Scanning networks");
			disableKeepAlive();
			std::vector<std::string> allNetworks = device.listNetworks();

			std::vector<std::string> networks;
			for (auto network : allNetworks)
			{
				if (!alreadyScanned(network) && network.find("CPEP::") == 0)
				{
					networks.push_back(network);
				}
			}

			serial->sendBytes(device::ScanStartedSerializer::serialize(device::ScanStarted(provider->scanStartId, networks.size())));

			for (uint32_t index = 0; index < networks.size();++index )
			{
				const std::string & name = networks[index];
				finishedReading = false;
				targetDevice.reset();
				ConnectionData data = ConnectionData::defaultData;
				data.ssid = name;
				sockets::SocketHandle connection = device.connectTo(data);
				if (!connection)
				{
					LOG_WARNING("Could not connect to network %s", name.c_str());
					return;
				}
				connection->sendByte(sizeof(void*));

				parser::ParserDelegatorHandle delegator = buildDeviceDelegator(*this, name);
				parser::MessageBuilderHandle builder = std::make_shared<parser::MessageBuilder>(connection, delegator);

				LOG_INFO("Waiting for device details...");

				uint32_t attempts = 0;
				while (!finishedReading && attempts < 100)
				{
					++attempts;
					builder->pollAndReadTimes(100);
					device.delayT(100);
				}
				if (finishedReading) {
					LOG_INFO("Device details received, closing connection");
				}
				serial->sendBytes(device::ScanProgressSerializer::serialize(device::ScanProgress(provider->scanProgresId, index)));
				previouslyScanned.push_back(name);

				connection->shutdown();

			}
			serial->sendBytes(device::ScanFinishedSerializer::serialize(std::move(device::ScanFinished(provider->finishScanId))));
			enableKeepAlive();
		}

		bool AuthDevice::alreadyScanned(const std::string& name)
		{
			return  std::find(previouslyScanned.begin(), previouslyScanned.end(), name) != previouslyScanned.end();
		}
	}
}
