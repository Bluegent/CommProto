#ifndef ENDPOINT_UICONTROLLER_H
#define ENDPOINT_UICONTROLLER_H
#include <commproto/control/endpoint/Control.h>
#include <commproto/control/endpoint/ControlCollection.h>
#include <commproto/control/endpoint/Notification.h>

namespace commproto
{
	namespace control {
		namespace endpoint {

			class IdProvider;

			class UIController : public Control, public ControlCollection
			{
			public:
				UIController(const std::string & name) : Control{ name, 0 } {}
				virtual ~UIController() = default;
				virtual IdProvider& getIdProvider() = 0;
				virtual void send(Message msg) = 0;
				virtual void addNotification(const NotificationHandle& notification) = 0;
				virtual NotificationHandle getNotification(const uint32_t id) const = 0;
				virtual void displayNotification(const uint32_t id) const = 0;
				virtual void sendState() = 0;
			};

			using UIControllerHandle = std::shared_ptr<UIController>;

		}

	}
}


#endif // ENDPOINT_UICONTROLLER_H