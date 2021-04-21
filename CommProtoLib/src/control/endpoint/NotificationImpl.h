#ifndef ENDPOINT_NOTIFICATION_IMPL_H
#define ENDPOINT_NOTIFICATION_IMPL_H
#include <commproto/control/endpoint/Notification.h>
#include <commproto/control/endpoint/UIController.h>
#include <commproto/control/NotificationChains.h>
#include <map>

namespace commproto
{
	namespace control
	{
		namespace endpoint
		{
			class NotificationResponseHandler : public parser::Handler
			{
			public:
				NotificationResponseHandler(const endpoint::UIControllerHandle & controller_) : controller{ controller_ } {}
				void handle(messages::MessageBase&& data) override;
			private:
				UIControllerHandle controller;
			};


			class NotificationImpl : public Notification
			{
			public:
				NotificationImpl(const std::string& name, uint32_t id, const uint32_t notifId_, const uint32_t displayId_)
					: Notification(name, id)
					, notifId{ notifId_ }
					, displayId{ displayId_ }
					, actionCounter(0)
				{
				}

				Message serialize() const override;
				void addOption(const std::string& name) override;
				uint32_t addAction(const NotificationAction& action) override;
				void execute(const std::string& option, const uint32_t action) override;
				Message serializeDisplay(const std::string & text, uint32_t action) override;
			private:
				const uint32_t notifId;
				const uint32_t displayId;
				std::vector<std::string> options;
				std::map<uint32_t, NotificationAction> actions;
				uint32_t actionCounter;
			};
		}
	}

}

#endif //ENDPOINT_NOTIFICATION_IMPL_H