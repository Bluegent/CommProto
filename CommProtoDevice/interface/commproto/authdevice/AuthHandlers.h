#ifndef AUTH_HANDLERS_H
#define AUTH_HANDLERS_H
#include <commproto/parser/Handler.h>
#include <commproto/authdevice/AuthDeviceUtils.h>

namespace commproto
{
    namespace authdevice
    {

		class DeviceDataHandler : public parser::Handler
		{
		public:
			DeviceDataHandler(AuthRequestHandler& device_, const std::string & name_);
			void handle(messages::MessageBase&& data) override;
		private:
			AuthRequestHandler& device;
			std::string name;
		};

		class DeviceRejectandler : public parser::Handler
		{
		public:
			DeviceRejectandler(AuthRequestHandler& device_);
			void handle(messages::MessageBase&& data) override;
		private:
			AuthRequestHandler& device;
		};

		class DeviceAuthHandler : public parser::Handler
		{
		public:
			DeviceAuthHandler(AuthRequestHandler& device_);
			void handle(messages::MessageBase&& data) override;
		private:
			AuthRequestHandler& device;
		};

		class ScanHandler : public parser::Handler
		{
		public:
			ScanHandler(AuthRequestHandler& device_);

			void handle(messages::MessageBase&& data) override;
		private:
			AuthRequestHandler& device;
		};


		class RebootHandler : public parser::Handler
		{
		public:
			RebootHandler(AuthRequestHandler& device_);
			void handle(messages::MessageBase&& data) override;
		private:
			AuthRequestHandler& device;
		};

    }
}

#endif