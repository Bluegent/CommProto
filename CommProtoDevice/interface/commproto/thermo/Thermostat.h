#ifndef THERMO_H
#define THERMO_H
#include <commproto/endpointdevice/BaseEndpointAuth.h>
#include <commproto/thermo/ThermostatWrapper.h>

namespace commproto
{
	namespace thermo
	{
		class Thermostat : public endpointdevice::BaseEndpointAuth
		{
		public:
			Thermostat(endpointdevice::BaseEndpointWrapper& wrapper, const endpointdevice::DeviceDetails& details, ThermostateWrapper& thermo);

		protected:
			void setupDevice() override;
			void loopDevice() override;
		private:
			ThermostateWrapper & thermo;
			sockets::SocketHandle client;
		};
	}
}

#endif // THERMO_WRAPPER_H