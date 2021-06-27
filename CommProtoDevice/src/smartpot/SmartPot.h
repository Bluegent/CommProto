#ifndef  SMARTPOT_DEVICE_H
#define  SMARTPOT_DEVICE_H

#include "PotWrapper.h"

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
	namespace smartpot
	{
		class PotControl
		{
		public:
			virtual ~PotControl() = default;
			virtual	void togglePump(bool state) = 0;
			virtual void toggleUv(bool state) = 0;
		};

		struct Dependencies
		{
			sockets::SocketHandle client;
			endpoint::ChannelParserDelegatorHandle delegator;
			parser::MessageBuilderHandle builder;
			messages::TypeMapperHandle mapper;
		};

		class SmartPot : public endpointdevice::BaseEndpointAuth, public PotControl
		{
		public:
			SmartPot(endpointdevice::BaseEndpointWrapper& wrapper, const endpointdevice::DeviceDetails& details, PotWrapper& potDevice);
			void togglePump(bool state) override;
			void toggleUv(bool state) override;
		protected:
			void setupDevice() override;
			void loopDevice() override;
		private:


			void sendReadings() const;

			PotWrapper & potDevice;
			Dependencies dep;
			int tempTime;
			uint32_t then;
			uint32_t thenRst;
			uint32_t now;
			static const uint32_t totalMem;
			int32_t adjustState;
			static const uint32_t resetAmount;
			uint32_t tempMsgId;
			uint32_t soilMsgId;
			uint32_t uvMsgId;
			uint32_t noControllerId;
		};
	}
}




#endif