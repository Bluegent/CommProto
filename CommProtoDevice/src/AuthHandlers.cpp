#include <commproto/authdevice/AuthHandlers.h>
#include <commproto/device/AuthChains.h>
#include <commproto/logger/Logging.h>

namespace commproto
{
    namespace authdevice
    {

		DeviceDataHandler::DeviceDataHandler(AuthRequestHandler& device_, const std::string & name_)
			: device(device_)
			, name{ name_ }
		{
		}

		void DeviceDataHandler::handle(messages::MessageBase&& data)
		{
			device::DeviceDataMessage& msg = static_cast<device::DeviceDataMessage&>(data);
			EndpointData epData;
			epData.name = msg.prop;
			epData.manufacturer = msg.prop2;
			epData.description = msg.prop3;
			device.finishReading(epData, name);

		}

		DeviceRejectandler::DeviceRejectandler(AuthRequestHandler& device_)
			: device(device_)
		{
		}

		void DeviceRejectandler::handle(messages::MessageBase&& data)
		{
			device::DeviceAuthReject& msg = static_cast<device::DeviceAuthReject&>(data);
			LOG_INFO("Denied %s", msg.prop.c_str());
			device.responseDeny(msg.prop);
		}


		DeviceAuthHandler::DeviceAuthHandler(AuthRequestHandler& device_)
			: device(device_)
		{
		}

		void DeviceAuthHandler::handle(messages::MessageBase&& data)
		{
			device::DeviceAuthAccept& msg = static_cast<device::DeviceAuthAccept&>(data);
			LOG_INFO("Accepted %s", msg.prop.c_str());
			device.responseAccept(msg.prop, msg.prop2, msg.prop3);
		}

	    ScanHandler::ScanHandler(AuthRequestHandler& device_): device(device_)
	    {
	    }

	    void ScanHandler::handle(messages::MessageBase&& data)
	    {
		    device.scan();
	    }

	    RebootHandler::RebootHandler(AuthRequestHandler& device_)
			: device(device_)
	    {
	    }

	    void RebootHandler::handle(messages::MessageBase&& data)
	    {
			device.reboot();
	    }
    }
}
