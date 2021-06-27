#include "SmartPot.h"

#include <commproto/logger/Logging.h>
#include <commproto/control/endpoint/ParserDelegatorUtils.h>
#include <commproto/endpoint/DelegatorProvider.h>
#include <commproto/parser/ParserDelegatorUtils.h>
#include <commproto/control/endpoint/UIFactory.h>
#include <commproto/service/ServiceChains.h>
#include <commproto/endpoint/ChannelParserDelegator.h>
#include <commproto/endpoint/ParserDelegatorFactory.h>
#include <sstream>
#include <chrono>
#include <plant/interface/PlantMessages.h>
#include <commproto/control/ControllerChains.h>

namespace commproto
{
	namespace smartpot
	{
		class UvHandler : public parser::Handler
		{
		public:
			UvHandler(PotControl & control) : control(control) {}
			void handle(messages::MessageBase&& data) override;
		private:
			PotControl & control;
		};

		void UvHandler::handle(messages::MessageBase&& data)
		{
			auto msg = static_cast<plant::ToggleUVLamp&>(data);
			control.toggleUv(msg.prop);
		}

		class PumpHandler : public parser::Handler
		{
		public:
			PumpHandler(PotControl & control) : control(control) {}
			void handle(messages::MessageBase&& data) override;
		private:
			PotControl & control;
		};

		void PumpHandler::handle(messages::MessageBase&& data)
		{
			auto msg = static_cast<plant::TogglePump&>(data);
			control.togglePump(msg.prop);
		}


		class UxRequestHandler : public parser::Handler
		{
		public:
			UxRequestHandler(const uint32_t noControllerId, const sockets::SocketHandle & socket_)
				: noControllerMsgId(noControllerId)
				, socket(socket_)
			{
			}

			void handle(messages::MessageBase&& data) override;
		private:
			uint32_t noControllerMsgId;
			sockets::SocketHandle socket;
		};

		void UxRequestHandler::handle(messages::MessageBase&& data)
		{
			auto noConMsg = control::endpoint::NoControllerResponseSerializer::serialize(control::endpoint::NoControllerResponse(noControllerMsgId));
			socket->sendBytes(noConMsg);
			LOG_INFO("Sent reply that I don't have a controller");
		}

		parser::ParserDelegatorHandle buildSelfDelegator(const sockets::SocketHandle & socket_, PotControl & control, const uint32_t noControllerId)
		{
			std::shared_ptr<parser::ParserDelegator> delegator = std::make_shared<parser::ParserDelegator>();
			parser::DelegatorUtils::buildBase(delegator);
			parser::DelegatorUtils::addParserHandlerPair<plant::TogglePumpParser, plant::TogglePump>(delegator, std::make_shared<PumpHandler>(control));
			parser::DelegatorUtils::addParserHandlerPair<plant::ToggleUVLampParser, plant::ToggleUVLamp>(delegator, std::make_shared<UvHandler>(control));
			parser::DelegatorUtils::addParserHandlerPair<control::ux::RequestControllerStateParser, control::ux::RequestControllerState>(delegator, std::make_shared<UxRequestHandler>(noControllerId, socket_));
			return delegator;
		}

		class SmartpotProvider : public endpoint::DelegatorProvider {
		public:
			SmartpotProvider(const sockets::SocketHandle & socket, PotControl & pot, const uint32_t noControllerId)
				: socket{ socket }
				, control(pot)
				, noControllerId(noControllerId)
			{

			}
			parser::ParserDelegatorHandle provide(const std::string& name, const uint32_t id) override
			{
				parser::ParserDelegatorHandle delegator = buildSelfDelegator(socket, control, noControllerId);

				return delegator;
			}
		private:
			sockets::SocketHandle socket;
			PotControl & control;
			uint32_t noControllerId;
		};

		const uint32_t SmartPot::totalMem = 520 * 1024;

		SmartPot::SmartPot(endpointdevice::BaseEndpointWrapper& wrapper, const endpointdevice::DeviceDetails& details, PotWrapper& potDevice)
			: BaseEndpointAuth(wrapper, details)
			, potDevice(potDevice)
			, tempTime(0)
			, then(0)
			, thenRst(0)
			, now(0)
			, adjustState(0)
		{
		}

		void SmartPot::setupDevice()
		{
			dep.client = tryConnect();
			if (!dep.client)
			{
				LOG_INFO("All connection attempts failed, restarting");
				device.reboot();
				return;
			}
			potDevice.setupBoard();
			device.blankLED();

			//intital setup
			dep.client->sendByte(sizeof(void*));
			device.delayT(10);
			std::string name = "Endpoint::SmartPot";
			SenderMapping::InitializeName(name);
			dep.mapper = messages::TypeMapperFactory::build(dep.client);

			//registering our name
			uint32_t registerId = dep.mapper->registerType<service::RegisterChannelMessage>();
			service::RegisterChannelMessage nameMsg(registerId, SenderMapping::getName());
			Message nameSerialized = service::RegisterChannelSerializer::serialize(std::move(nameMsg));
			dep.client->sendBytes(nameSerialized);
			device.delayT(10);

			tempMsgId = dep.mapper->registerType<plant::TempHum>();
			soilMsgId = dep.mapper->registerType<plant::Soil>();
			uvMsgId = dep.mapper->registerType<plant::UvLight>();
			noControllerId = dep.mapper->registerType<control::endpoint::NoControllerResponse>();

			//delegator and builder stuff
			std::shared_ptr<SmartpotProvider> provider = std::make_shared<SmartpotProvider>(dep.client, *this, noControllerId);
			dep.delegator = std::make_shared<endpoint::ChannelParserDelegator>(provider);
			parser::ParserDelegatorHandle delegator = endpoint::ParserDelegatorFactory::build(dep.delegator);
			dep.delegator->addDelegator(0, delegator);
			dep.builder = std::make_shared<parser::MessageBuilder>(dep.client, dep.delegator);



			LOG_INFO("Waiting to get our ID");
			device.delayT(50);
			//wait until we have an ID
			do
			{
				dep.builder->pollAndRead();
				device.delayT(10);
			} while (SenderMapping::getId() == 0);

			LOG_INFO("Succesfully set ID to %d", SenderMapping::getId());

		}
		static bool notLogged = true;

		void SmartPot::loopDevice()
		{
			if (!dep.client)
			{
				LOG_INFO("Something went wrong during device setup, please reset.");
				return;
			}
			potDevice.loop();

			now = potDevice.getMs();
			uint32_t diff = now - then;
			if (diff > 4000) {
				then = now;
				potDevice.getReadings();
				sendReadings();

			}

			diff = now - thenRst;
			if (diff > 1000)
			{
				thenRst = now;
				checkResetState();
			}

			if (dep.builder)
			{
				uint32_t bytes = dep.client->available();
				if (bytes) {
					dep.builder->pollAndRead();
				}
			}

			if (!dep.client->connected() && notLogged)
			{
				notLogged = false;
				LOG_INFO("Disconnected :(");
				device.reboot();
			}

		}

		void SmartPot::togglePump(bool state)
		{
			LOG_DEBUG("Toggling pump %s", state ? "on" : "off");
			potDevice.togglePump(state);
		}

		void SmartPot::toggleUv(bool state)
		{
			LOG_DEBUG("Toggling lamp %s", state ? "on" : "off");
			potDevice.toggleLamp(state);
		}

		void SmartPot::sendReadings() const
		{
			LOG_DEBUG("Sending readings");
			float temperature = potDevice.getTemperature();
			float humidity = potDevice.getHumidity();
			uint32_t soil = potDevice.getSoilHumidity();
			uint32_t uv = potDevice.getLightExposure();

			dep.client->sendBytes(plant::TempHumSerializer::serialize(plant::TempHum(tempMsgId, temperature, humidity)));
			dep.client->sendBytes(plant::SoilSerializer::serialize(plant::Soil(soilMsgId, soil)));
			dep.client->sendBytes(plant::UvLightSerializer::serialize(plant::UvLight(uvMsgId, uv)));
		}
	}
}
