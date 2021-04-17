#include <commproto/device/AuthChains.h>
#include <commproto/logger/Logging.h>
#include <commproto/parser/ParserDelegatorUtils.h>
#include <commproto/endpointdevice/BaseEndpointAuth.h>

namespace commproto
{
	namespace endpointdevice
	{
		parser::ParserDelegatorHandle build(BaseEndpointAuth& device)
		{
			parser::ParserDelegatorHandle delegator = std::make_shared<parser::ParserDelegator>();
			parser::buildBase(delegator);
			parser::addParserHandlerPair<device::ConnectionAuthorizedParser, device::ConnectionAuthorizedMessage>(delegator, std::make_shared<AcceptHandler>(device));
			parser::addParserHandlerPair<device::ConnectionRejectedParser, device::ConnectionRejectedMessage>(delegator, std::make_shared<RejectHandler>(device));


			return delegator;
		}

		BaseEndpointAuth::BaseEndpointAuth(BaseEndpointWrapper& wrapper, const DeviceDetails& details)
			: device(wrapper)
			, state{ BaseAuthState::SendAuthData }
			, responseAttempts{ 0 }
			, thisDevice(details)
			, isAP(true)
		{

		}

		void BaseEndpointAuth::setup()
		{
			
			serial = device.getStream(115200);
			device.initFs();
			isAP = !device.readAPData();

			LOG_INFO();
			LOG_INFO("Starting base endpoint setup...");
			if (isAP)
			{
				LOG_INFO("Failed to read auth data from memory, starting as access point.");
				setupAP();
			}
			else
			{
				auto data = device.getAuthData();
				LOG_INFO("Starting as client to %s(pwd:%s), dispatch: %s:%d",data.ssid.c_str(),data.password.c_str(),data.addr.c_str(),data.port);
				setupDevice();
			}
		}

		void BaseEndpointAuth::loop()
		{
			if (isAP)
			{
				loopAP();
			}
			else
			{
				loopDevice();
			}
		}



		void BaseEndpointAuth::setupAP()
		{
		}



		void BaseEndpointAuth::loopAP()
		{
			switch (state)
			{
			case BaseAuthState::SendAuthData:
			{
				if (!socket)
				{

					socket = device.startAsAP(authdevice::ConnectionData::defaultData);
					LOG_INFO("Waiting for client");
				}
				sockets::SocketHandle client = socket->acceptNext();
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
				mapper = messages::TypeMapperFactory::build(client);
				delegator = build(*this);

				uint32_t authMsgId = mapper->registerType<device::DeviceDataMessage>();
				Message authMsg = device::DeviceDataSerializer::serialize(std::move(device::DeviceDataMessage(authMsgId, thisDevice.name, thisDevice.manufacturer, thisDevice.description)));
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
				sockets::SocketHandle reconnect = socket->acceptNext();
				if (responseAttempts % 100 == 0) {
					LOG_INFO("Attempt #%d waiting for auth connection", responseAttempts / 100);
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
				builder = std::make_shared<parser::MessageBuilder>(reconnect, delegator);

				responseAttempts = 0;
				LOG_INFO("Waiting for authetification response");

				while (responseAttempts <= 10)
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

		void BaseEndpointAuth::setupDevice()
		{
		}

		void BaseEndpointAuth::loopDevice()
		{
			LOG_INFO("Pretend I'm a device :)");
			device.delayT(10000);
		}

		void BaseEndpointAuth::accept(const authdevice::ConnectionData& data)
		{
			device.saveAPData(data);
			LOG_INFO("Got authentification data");
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
	}
}