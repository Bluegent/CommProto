#ifndef UX_UICONTROLLER_H
#define UX_UICONTROLLER_H
#include <commproto/control/ux/Control.h>
#include <commproto/control/ux/ControlCollection.h>
#include <commproto/control/ux/Notification.h>

namespace commproto
{
	namespace control {
		namespace ux {

			class IdProvider;


			class UIController : public Control, public ControlCollection
			{
			public:
				UIController(const std::string & name) : Control{ name,0 } {}
				virtual std::string getConnectionName() = 0;
				virtual uint32_t getConnectionId() = 0;
				virtual ~UIController() = default;
				virtual IdProvider& getIdProvider() = 0;
				virtual void send(Message msg) = 0;
				virtual void addNotification(const NotificationHandle& notification) = 0;
				virtual NotificationHandle getNotification(const uint32_t id) const = 0;
				virtual void displayNotification(const uint32_t id) = 0;
				virtual bool hasNotifications() = 0;
				virtual std::string getNotifications() = 0;
				virtual void dismissNotification(const uint32_t id) = 0;
				virtual void requestState() = 0;
			};

			using UIControllerHandle = std::shared_ptr<UIController>;
		}
	}
}


#endif // UICONTROLLER_H