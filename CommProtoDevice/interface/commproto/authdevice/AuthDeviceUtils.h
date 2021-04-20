#ifndef AUTH_DEVICE_UTILS_H
#define AUTH_DEVICE_UTILS_H

#include <string>
#include <commproto/device/AuthChains.h>
#include <commproto/messages/TypeMapper.h>
#include <commproto/authdevice/Structures.h>

namespace commproto
{
	namespace authdevice
	{


		struct IdProvider
		{
			IdProvider(const messages::TypeMapperHandle &mapper)
				: deviceAuthId{ mapper->registerType<device::DeviceAuthRequestMessage>() }
				, finishScanId{ mapper->registerType<device::ScanFinished>() }
				, keepAliveId{ mapper->registerType<device::KeepAlive>() }
			{
			}
			const uint32_t deviceAuthId;
			const uint32_t finishScanId;
			const uint32_t keepAliveId;
		};

		using ProviderHandle = std::shared_ptr<IdProvider>;


		class AuthRequestHandler
		{
		public:
			virtual ~AuthRequestHandler() = default;
			virtual void finishReading(const EndpointData& data, const std::string & name) = 0;
			virtual void scan() = 0;
			virtual void responseAccept(const std::string & name, const std::vector<std::string> & reply, const uint32_t port) = 0;
			virtual void responseDeny(const std::string & name) = 0;
			virtual void reboot() = 0;
			virtual void gotPong() = 0;
		};
	}
}

#endif// AUTH_DEVICE_UTILS_H