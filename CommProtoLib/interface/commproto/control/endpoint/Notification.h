#ifndef ENDPOINT_NOTIFICATION_H
#define ENDPOINT_NOTIFICATION_H
#include <commproto/control/endpoint/Control.h>
#include <functional>

namespace commproto
{
	namespace control
	{
		namespace endpoint
		{

			using NotificationAction = std::function<void(const std::string &)>;
			class Notification : public Control
			{
			public:
				Notification(const std::string& name, uint32_t id)
					: Control(name, id)
				{
				}
				virtual void addOption(const std::string & name) = 0;
				virtual uint32_t addAction(const NotificationAction& action) = 0;
				virtual void execute(const std::string & option, const uint32_t action) = 0;
				virtual Message serializeDisplay(const std::string & text, const uint32_t action) = 0;
			};

			using NotificationHandle = std::shared_ptr<Notification>;
		}
	}
}

#endif //ENDPOINT_NOTIFICATION_H