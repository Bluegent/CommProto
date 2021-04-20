#include <commproto/thermo/Thermostat.h>
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

namespace commproto
{
	namespace thermo
	{

		parser::ParserDelegatorHandle buildSelfDelegator()
		{
			std::shared_ptr<parser::ParserDelegator> delegator = std::make_shared<parser::ParserDelegator>();
			parser::DelegatorUtils::buildBase(delegator);
			return delegator;
		}


		class EndpointProvider : public endpoint::DelegatorProvider {
		public:
			EndpointProvider(const messages::TypeMapperHandle & mapper_, const control::endpoint::UIControllerHandle & controller_)
				: mapper{ mapper_ }
				, controller{ controller_ }
			{

			}
			parser::ParserDelegatorHandle provide(const std::string& name, const uint32_t id) override
			{
				parser::ParserDelegatorHandle delegator = buildSelfDelegator();
				control::endpoint::DelegatorUtils::addParsers(delegator, controller);

				return delegator;
			}
		private:
			messages::TypeMapperHandle mapper;
			control::endpoint::UIControllerHandle controller;
		};


		Thermostat::Thermostat(endpointdevice::BaseEndpointWrapper& wrapper, const endpointdevice::DeviceDetails& details, ThermostateWrapper& thermo)
			: BaseEndpointAuth(wrapper, details)
			, thermo(thermo)
			, tempTime(0)
			, then(0)
			, now(0)
		{
		}

		void Thermostat::setupDevice()
		{
			dep.client = tryConnect();
			if (!dep.client)
			{
				LOG_INFO("All connection attempts failed, ressetting configuration and restarting in AP mode");
				device.resetAPData();
				device.reboot();
				return;
			}
			thermo.setup();

			//intital setup
			dep.client->sendByte(sizeof(void*));
			device.delayT(100);
			SenderMapping::InitializeName("CPEP::Thermostat");
			dep.mapper = messages::TypeMapperFactory::build(dep.client);

			//ui stuff
			auto uiFactory = std::make_shared<control::endpoint::UIFactory>("myUI", dep.mapper, dep.client);
			dep.controller = uiFactory->makeController();

			ui.tempLabel = uiFactory->makeLabel("Temperature", "0.00 C");
			ui.humLabel = uiFactory->makeLabel("Humidity", "0.00 %");
			dep.controller->addControl(ui.tempLabel);
			dep.controller->addControl(ui.humLabel);

			ui.autoTemp = false;
			control::endpoint::ToggleAction toggleMode = std::bind(&Thermostat::toggleAutoTemp, this, std::placeholders::_1);
			ui.toggleAutoTemp = uiFactory->makeToggle("Automatic Temperature", toggleMode);
			dep.controller->addControl(ui.toggleAutoTemp);


			control::endpoint::SliderAction intensityAction = std::bind(&Thermostat::setAdjustIntensity, this, std::placeholders::_1);
			ui.intensitySlider = uiFactory->makeSlider("Heat/Cool", intensityAction);
			ui.intensitySlider->setInitialValue(0.f);
			ui.intensitySlider->setLimits(-4.f, 4.f);
			ui.intensitySlider->setStep(1.f);
			dep.controller->addControl(ui.intensitySlider);

			control::endpoint::SliderAction desiredtempAction = std::bind(&Thermostat::setDesiredTemp, this, std::placeholders::_1);
			ui.desiredTempSlider = uiFactory->makeSlider("Desired Temperature:", desiredtempAction);
			ui.desiredTempSlider->setInitialValue(25.f);
			ui.desiredTempSlider->setLimits(5.f, 38.f);
			ui.desiredTempSlider->setStep(0.5f);
			dep.controller->addControl(ui.desiredTempSlider);
			thermo.setDesiredTemp(25.f);



			//registering our name
			uint32_t registerId = dep.mapper->registerType<service::RegisterChannelMessage>();
			service::RegisterChannelMessage nameMsg(registerId, SenderMapping::getName());
			Message nameSerialized = service::RegisterChannelSerializer::serialize(std::move(nameMsg));
			dep.client->sendBytes(nameSerialized);
			device.delayT(100);

			//delegator and builder stuff
			std::shared_ptr<EndpointProvider> provider = std::make_shared<EndpointProvider>(dep.mapper, dep.controller);
			dep.delegator = std::make_shared<endpoint::ChannelParserDelegator>(provider);
			parser::ParserDelegatorHandle delegator = endpoint::ParserDelegatorFactory::build(dep.delegator);
			dep.delegator->addDelegator(0, delegator);
			dep.builder = std::make_shared<parser::MessageBuilder>(dep.client, dep.delegator);


			LOG_INFO("Waiting to get our ID");
			device.delayT(100);
			//wait until we have an ID
			do
			{
				dep.builder->pollAndRead();
				device.delayT(100);
			} while (SenderMapping::getId() == 0);

			dep.controller->setControlShownState(ui.desiredTempSlider->getId(), false);

			LOG_INFO("Succesfully set ID to %d", SenderMapping::getId());

		}
		static bool notLogged = true;

		void Thermostat::loopDevice()
		{
			if (!dep.client)
			{
				LOG_INFO("Something went wrong during device setup, please reset.");
				return;
			}
			
			
			now = thermo.getMs();
			uint32_t diff = now - then;
			if (diff > 5000) {
				thermo.loop();
				then = now;
				
				tempTime = 0;
				float temp = thermo.getTemp();
				float humidity = thermo.getHumidity();

				std::stringstream tempStr;
				tempStr.precision(3);
				tempStr << temp << " C";
				ui.tempLabel->setText(tempStr.str());

				std::stringstream humStr;
				humStr.precision(3);

				humStr << humidity << " %";
				ui.humLabel->setText(humStr.str());
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
			}

		}

		void Thermostat::toggleAutoTemp(const bool on)
		{
			LOG_INFO("Setting auto temperature adjustment %s", on ? "on" : "off");
			thermo.toggleAutoTempAdjust(on);

			dep.controller->setControlShownState(ui.desiredTempSlider->getId(), on);
			dep.controller->setControlShownState(ui.intensitySlider->getId(), !on);
			thermo.loop();

		}

		void Thermostat::setDesiredTemp(const float temp)
		{
			LOG_INFO("Desired temperature set to %.2f", temp);
			thermo.setDesiredTemp(temp);
			thermo.loop();
		}

		void Thermostat::setAdjustIntensity(const float temp)
		{
			LOG_INFO("Temperature adjust set to %.0f", temp);
			thermo.toggleAutoTempAdjust(false);
			thermo.toggleTempAdjust(temp);			
		}
	}
}
