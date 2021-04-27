#ifndef AUTH_DEVICE_H
#define AUTH_DEVICE_H
#include <commproto/authdevice/AuthDeviceWrapper.h>
#include <commproto/logger/Logging.h>
#include <commproto/messages/TypeMapper.h>
#include <commproto/parser/MessageBuilder.h>
#include <commproto/serial/LogToSerial.h>
#include <commproto/authdevice/AuthDeviceUtils.h>
#include <commproto/authdevice/Structures.h>

namespace commproto
{
	namespace authdevice
	{
		class AuthDevice : public AuthRequestHandler
		{
		public:
			AuthDevice(AuthDeviceWrapper& device);
			void setup();
			void loop();
			void finishReading(const EndpointData& data, const std::string & name) override;
			void scan() override;
			void responseAccept(const std::string & name, const std::vector<std::string> & reply, const uint32_t port) override;
			void responseDeny(const std::string & name) override;
			void reboot() override;
			void sendPing();
			void gotPong() override;
			void disableKeepAlive();
			void enableKeepAlive();
		private:
			void scanNetworks();
			bool alreadyScanned(const std::string & name);
		private:
			AuthDeviceWrapper& device;
			bool finishedReading;
			EndpointData targetDevice;
			bool serviceConnected;
			serial::SerialHandle serial;
			parser::MessageBuilderHandle builder;
			messages::TypeMapperHandle mapper;
			ProviderHandle provider;
			serial::LogToSerialHandle log;
			bool shouldScan;
			std::vector<std::string> previouslyScanned;
			uint32_t lastPing;
			uint32_t lastPong;
			uint32_t lastCheck;
			bool keepAliveOn;
		};
	}
}
#endif // AUTH_DEVICE_H