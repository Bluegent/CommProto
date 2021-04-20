#ifndef UX_UICONTROLLER_IMPL_H
#define UX_UICONTROLLER_IMPL_H

#include <commproto/control/ux/UIController.h>
#include <commproto/sockets/Socket.h>
#include <commproto/control/ux/TemplateEngine.h>
#include <map>
#include "UpdateTracker.h"
#include "IdProvider.h"
#include <mutex>
#include <atomic>

namespace commproto
{
	namespace control
	{
		namespace ux
		{
			class UIControllerImpl : public UIController
			{
			public:
				UIControllerImpl(const std::string& name, const std::string& connectionName_, const messages::TypeMapperHandle & mapper, const sockets::SocketHandle & socket, const uint32_t id, const TemplateEngineHandle& engine_);
				void addControl(const ControlHandle& control) override;
				std::string getConnectionName() const override;
				std::string getUx() override;
				void send(Message msg) override;
				~UIControllerImpl();
				IdProvider& getIdProvider() override;
				uint32_t getConnectionId() override;
				ControlHandle getControl(const uint32_t id) override;
				bool hasUpdate(const std::string& tracker) override;
				void clear() override;
				void notifyUpdate(const uint32_t &controlId) override;
				void addNotification(const NotificationHandle& notification) override;
				NotificationHandle getNotification(const uint32_t id) const override;
				void displayNotification(const uint32_t id) override;
				bool hasNotifications() override;
				std::string getNotifications() override;
				void dismissNotification(const uint32_t id) override;
				void requestState() override;
				TemplateEngineHandle getEngine() override;
				void addTracker(const std::string& addr) override;
				std::map<std::string, std::string> getUpdates(const std::string& addr, bool force) override;
				void startCheckingTrackers() override;
			private:

				void checkTrackers();
				std::string UIControllerImpl::getControlId(const uint32_t control) const;
				
				std::map<uint32_t, ControlHandle> controls;
				std::map<uint32_t, NotificationHandle> notifications;
				const std::string connectionName;
				IdProvider provider;
				sockets::SocketHandle socket;
				uint32_t connectionId;
				std::mutex controlMutex;
				std::atomic_bool update;
				std::atomic_bool hasNotif;
				std::vector<uint32_t> pendingNotifications;
				std::mutex notificationMutex;
				TemplateEngineHandle engine;
				std::map<std::string, UpdateTrackerHandle> trackers;
				std::shared_ptr<std::thread> checkTrackersThread;
				std::atomic_bool checkingTrackers;
			};


			class ControlStateHandler : public parser::Handler
			{
			public:
				ControlStateHandler(const UIControllerHandle & controller_);
				void handle(messages::MessageBase&& data) override;
			private:
				UIControllerHandle controller;
			};


			class ControlShownHandler : public parser::Handler
			{
			public:
				ControlShownHandler(const UIControllerHandle & controller_);
				void handle(messages::MessageBase&& data) override;
			private:
				UIControllerHandle controller;
			};
		}
	}
}


#endif // UX_UICONTROLLER_IMPL_H