#include "NotificationImpl.h"
#include "IdProvider.h"

namespace commproto
{
	namespace control
	{

		namespace ux
		{
			void NotificationHandler::handle(messages::MessageBase&& data)
			{
				endpoint::NotificationMessage& msg = static_cast<endpoint::NotificationMessage&>(data);
				ux::NotificationHandle notification = std::make_shared<ux::NotificationImpl>(msg.prop2,
					msg.prop,
					controller->getIdProvider().notificationResponseId,
					msg.prop3,
					controller->getGenerator());
				controller->addNotification(notification);
			}

			void DisplayNotificationHandler::handle(messages::MessageBase&& data)
			{
				endpoint::DisplayNotification& msg = static_cast<endpoint::DisplayNotification&>(data);
				controller->displayNotification(msg.prop, msg.prop3, msg.prop2);
			}

			UxContainerHandle NotificationImpl::getUx()
			{
				return UxContainerHandle{};
			}

			void NotificationImpl::execute(const std::string& option, const uint32_t actionId)
			{
				Message msg = NotificationResponseSerializer::serialize(std::move(NotificationResponse(executeId, id, actionId, option)));
				generator->send(std::move(msg));
			}

			std::vector<std::string> NotificationImpl::getOptions() const
			{
				return options;
			}

			UxContainerHandle NotificationImpl::getUx(const std::string& text, const uint32_t action) const
			{
				return generator->generateNotification(*this, text, action);
			}
		}
	}
}