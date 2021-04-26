#ifndef THERMO_H
#define THERMO_H
#include <commproto/endpointdevice/BaseEndpointAuth.h>
#include <commproto/thermo/ThermostatWrapper.h>
#include <commproto/control/endpoint/UIController.h>
#include <commproto/control/endpoint/Label.h>
#include <commproto/control/endpoint/Slider.h>
#include <commproto/control/endpoint/Toggle.h>
#include <commproto/endpoint/ChannelParserDelegator.h>
#include <chrono>


namespace commproto
{
	namespace thermo
	{

		struct Dependencies
		{
			sockets::SocketHandle client;
			endpoint::ChannelParserDelegatorHandle delegator;
			parser::MessageBuilderHandle builder;
			messages::TypeMapperHandle mapper;
			control::endpoint::UIControllerHandle controller;
		};

		struct UI
		{
			control::endpoint::LabelHandle tempLabel;
			control::endpoint::LabelHandle humLabel;
			control::endpoint::SliderHandle intensitySlider;
			control::endpoint::SliderHandle desiredTempSlider;
			control::endpoint::ToggleHandle toggleAutoTemp;
			bool autoTemp;
		};

		class Thermostat : public endpointdevice::BaseEndpointAuth
		{
		public:
			Thermostat(endpointdevice::BaseEndpointWrapper& wrapper, const endpointdevice::DeviceDetails& details, ThermostateWrapper& thermo);

		protected:
			void setupDevice() override;
			void loopDevice() override;
		private:

			void toggleAutoTemp(const bool on);
			void setDesiredTemp(const float temp);
			void setAdjustIntensity(const float temp);


			ThermostateWrapper & thermo;
			Dependencies dep;
			UI ui;
			int tempTime;
			uint32_t then;
			uint32_t now;
			static const uint32_t totalMem;
			uint32_t adjustState;
			

		};
	}
}

#endif // THERMO_WRAPPER_H