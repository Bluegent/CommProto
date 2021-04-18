#include <commproto/thermo/Thermostat.h>
#include "commproto/logger/Logging.h"

namespace commproto
{
	namespace thermo
	{
		Thermostat::Thermostat(endpointdevice::BaseEndpointWrapper& wrapper, const endpointdevice::DeviceDetails& details, ThermostateWrapper& thermo)
			: BaseEndpointAuth(wrapper, details)
			, thermo(thermo)
		{
		}

		void Thermostat::setupDevice()
		{
			client = tryConnect();
			if(!client)
			{
				LOG_INFO("All connection attempts failed, ressetting configuration and restarting in AP mode");
				//device.resetAPData();
				device.reboot();
			}
			
		}

		void Thermostat::loopDevice()
		{
			if(!client)
			{
				LOG_INFO("Something went wrong during device setup, please reset.");
				return;
			}

			int32_t temp = thermo.getTemp();
			int32_t humidity = thermo.getHumidity();

			//send them to UI updates

			thermo.loop();

			device.delayT(5000);

		}
	}
}
