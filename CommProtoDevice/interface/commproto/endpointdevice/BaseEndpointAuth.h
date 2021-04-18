#ifndef BASE_ENDPOINT_AUTH_H
#define BASE_ENDPOINT_AUTH_H

#include <commproto/endpointdevice/BaseEndpointWrapper.h>
#include <commproto/messages/TypeMapper.h>
#include <commproto/parser/MessageBuilder.h>
#include <commproto/authdevice/Structures.h>
#include <commproto/endpointdevice/BaseEndpointAuthHandlers.h>

namespace commproto
{
	namespace endpointdevice {

		enum class BaseAuthState
		{
			SendAuthData,
			WaitForReconnect,
			ReadForResponse,
		};

		struct DeviceDetails
		{
			std::string name;
			std::string manufacturer;
			std::string description;
		};

		class BaseEndpointAuth : public EndpointAuthRequestHandler
		{
		public:
			BaseEndpointAuth(BaseEndpointWrapper& wrapper, const DeviceDetails& details);
			void setup();
			void loop();
			void accept(const authdevice::ConnectionData& data) override;
			void reject() override;
		protected:
			virtual void setupDevice();
			virtual void loopDevice();
			sockets::SocketHandle tryConnect(const uint32_t attempts = 10) const;
			bool isAP;
			BaseEndpointWrapper& device;

		private:
			void setupAP();
			void loopAP();

			
			stream::StreamHandle serial;
			BaseAuthState state;
			sockets::SocketHandle socket;
			messages::TypeMapperHandle mapper;
			parser::MessageBuilderHandle builder;
			parser::ParserDelegatorHandle delegator;
			uint32_t responseAttempts;
			const DeviceDetails thisDevice;
			

		};
	}
}
#endif //BASE_ENDPOINT_AUTH_H