#ifndef UX_UICONTROLLER_H
#define UX_UICONTROLLER_H

#include <commproto/control/ux/Control.h>
#include <commproto/control/ux/ControlCollection.h>
#include <commproto/control/ux/Notification.h>
#include <commproto/control/ux/TemplateEngine.h>
namespace commproto
{
	namespace control {
		namespace ux {

			class IdProvider;

			using UpdateMap = std::map<std::string, std::string>;

			class UIController : public Control, public ControlCollection
			{
			public:
				UIController(const std::string & name) : Control{ name,0 } {}
				virtual std::string getConnectionName() const = 0;
				virtual uint32_t getConnectionId() = 0;
				virtual ~UIController() = default;
				virtual IdProvider& getIdProvider() = 0;
				virtual void send(Message msg) = 0;
				virtual void addNotification(const NotificationHandle& notification) = 0;
				virtual NotificationHandle getNotification(const uint32_t id) const = 0;
				virtual void displayNotification(const uint32_t id, const std::string & text, const uint32_t actionId) = 0;
				virtual bool hasNotifications(const std::string & tracker) = 0;
				virtual UpdateMap getNotifications(const std::string & tracker, const bool force) = 0;
				virtual void dismissNotification(const uint32_t id, const uint32_t actiondId) = 0;
				virtual void requestState() = 0;
				virtual TemplateEngineHandle getEngine() = 0;
				virtual void addTracker(const std::string & addr) = 0;
				virtual UpdateMap getUpdates(const std::string & addr, bool force = false) = 0;
				virtual void startCheckingTrackers() = 0;
				virtual std::string getControlId(const uint32_t control, const std::string & controlType = std::string{}) const = 0;
				virtual void notifyTrackerUpdate(const std::string & tracker, const uint32_t control) = 0;
			};

			using UIControllerHandle = std::shared_ptr<UIController>;
		}
	}
}


#endif // UICONTROLLER_H