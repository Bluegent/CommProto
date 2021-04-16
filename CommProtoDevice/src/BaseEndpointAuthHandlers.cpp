#include <commproto/endpointdevice/BaseEndpointAuthHandlers.h>
#include <commproto/device/AuthChains.h>
#include <commproto/logger/Logging.h>


namespace commproto
{
	namespace endpointdevice
	{


		AcceptHandler::AcceptHandler(EndpointAuthRequestHandler& thermo_)
			: device(thermo_)
		{
		}

		void AcceptHandler::handle(messages::MessageBase&& data)
		{
			device::ConnectionAuthorizedMessage& msg = static_cast<device::ConnectionAuthorizedMessage&>(data);
			if (msg.prop.size() != 3)
			{
				LOG_ERROR("Connection authorization message has too few arguments");
				device.reject();
			}
			device.accept(authdevice::ConnectionData{ msg.prop[0], msg.prop[1],msg.prop[2],msg.prop2 });
		}

		RejectHandler::RejectHandler(EndpointAuthRequestHandler& thermo_)
			: device(thermo_)
		{
		}

		void RejectHandler::handle(messages::MessageBase&& data)
		{
			device.reject();
		}
	}
}
