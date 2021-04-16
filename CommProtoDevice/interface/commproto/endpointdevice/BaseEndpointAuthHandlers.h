#ifndef BASE_ENDPOINT_AUTH_HANDLERS_H
#define BASE_ENDPOINT_AUTH_HANDLERS_H
#include <commproto/parser/Handler.h>
#include <commproto/authdevice/Structures.h>


namespace commproto
{
	namespace endpointdevice 
	{
		class EndpointAuthRequestHandler
		{
		public:
			virtual ~EndpointAuthRequestHandler() = default;
			virtual void accept(const authdevice::ConnectionData& data) = 0;
			virtual void reject() = 0;
		};

		class AcceptHandler : public parser::Handler
		{
		public:
			AcceptHandler(EndpointAuthRequestHandler& thermo_);
			void handle(messages::MessageBase&& data) override;
		private:
			EndpointAuthRequestHandler& device;
		};

		class RejectHandler : public parser::Handler
		{
		public:
			RejectHandler(EndpointAuthRequestHandler& thermo_);
			void handle(messages::MessageBase&& data) override;
		private:
			EndpointAuthRequestHandler& device;
		};
	}
}
#endif //BASE_ENDPOINT_AUTH_HANDLERS_H