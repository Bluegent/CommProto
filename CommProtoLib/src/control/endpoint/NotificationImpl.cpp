#include "NotificationImpl.h"
#include "IdProvider.h"

namespace commproto
{
	namespace control
	{
		namespace endpoint
		{

			void NotificationResponseHandler::handle(messages::MessageBase&& data)
			{
				ux::NotificationResponse& msg = static_cast<ux::NotificationResponse&>(data);

				NotificationHandle notification = controller->getNotification(msg.prop);
				if (!notification)
				{
					return;
				}
				notification->execute(msg.prop3, msg.prop2);

			}

			Message NotificationImpl::serialize() const
			{
				return NotificationSerializer::serialize(std::move(NotificationMessage(notifId, id, name, options)));
			}

			void NotificationImpl::addOption(const std::string& name)
			{
				options.push_back(name);
			}

			uint32_t NotificationImpl::addAction(const NotificationAction& action_)
			{
				uint32_t id = actionCounter++;
				actions.emplace(id, action_);
				return id;
			}

			void NotificationImpl::execute(const std::string& option, const uint32_t actionId)
			{
				auto action = actions.find(actionId);
				if (action == actions.end())
				{
					return;
				}
				action->second(option);
				actions.erase(action);
			}

			Message NotificationImpl::serializeDisplay(const std::string & text, uint32_t action)
			{
				return DisplayNotificationSerializer::serialize(std::move(DisplayNotification(displayId, id, action, text)));
			}
		}
	}
}