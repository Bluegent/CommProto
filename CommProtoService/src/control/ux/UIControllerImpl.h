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

			class Generator;
			using GeneratorHandle = std::shared_ptr<Generator>;

			struct NotificationData
			{
				NotificationData(const std::string& text, uint32_t control_id)
					: text(text)
					, controlId(control_id)
				{
				}

				const std::string text;
				const uint32_t controlId;
			};

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
				void displayNotification(const uint32_t id, const std::string & text, const uint32_t actionId) override;
				bool hasNotifications(const std::string & tracker) override;
				UpdateMap getNotifications(const std::string & tracker, const bool force) override;
				void dismissNotification(const std::string & tracker, const uint32_t actiondId) override;
				void requestState() override;
				TemplateEngineHandle getEngine() override;
				void addTracker(const std::string& addr) override;
				UpdateMap getUpdates(const std::string& addr, bool force) override;
				void startCheckingTrackers() override;
				std::string getControlId(const uint32_t control, const std::string& controlType = std::string{}) const override;
				void notifyTrackerUpdate(const std::string& tracker, const uint32_t control) override;
				Removals getRemovals(const std::string& tracker) override;
				messages::TypeMapperHandle getMapper() const override;
			private:
				void checkTrackers();
			public:
				void addControlHandler(const std::string & extensionName, const ControlHandlerHandle& handler) override;
				void handle(AttributeMap&& attributes) override;
			private:
				std::map<uint32_t, ControlHandle> controls;
				std::map<uint32_t, NotificationHandle> notifications;
				const std::string connectionName;
				IdProvider provider;
				sockets::SocketHandle socket;
				messages::TypeMapperHandle mapper;
				uint32_t connectionId;
				std::mutex controlMutex;
				std::map<uint32_t, NotificationData> pendingNotifications;
				std::mutex notificationMutex;
				TemplateEngineHandle engine;
				std::map<std::string, UpdateTrackerHandle> trackers;
				std::map<std::string, UpdateTrackerHandle> notifTrackers;
				std::shared_ptr<std::thread> checkTrackersThread;
				std::atomic_bool checkingTrackers;
				GeneratorHandle generator;
				std::map<std::string, ControlHandlerHandle> handlers;
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