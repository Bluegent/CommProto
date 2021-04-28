#include "UIControllerImpl.h"
#include <sstream>
#include <commproto/logger/Logging.h>
#include <algorithm>
#include <commproto/control/ControllerChains.h>

#include "UxGenerator.h"

namespace commproto
{
	namespace control {

		namespace ux
		{

			UIControllerImpl::UIControllerImpl(
				const std::string& name, const std::string& connectionName_
				, const messages::TypeMapperHandle & mapper
				, const sockets::SocketHandle & socket_
				, const uint32_t id
				, const TemplateEngineHandle & engine_)
				: UIController{ name }
				, connectionName{ connectionName_ }
				, provider{ mapper }
				, socket{ socket_ }
				, connectionId{ id }
				, engine{ engine_ }
				, checkingTrackers(false)
				, generator{ std::make_shared<Generator>(*this) }
			{
			}

			void UIControllerImpl::addControl(const ControlHandle& control)
			{
				std::lock_guard<std::mutex> lock(controlMutex);
				const uint32_t id = control->getId();
				if (controls.find(id) != controls.end())
				{
					return;
				}
				controls.emplace(id, control);
				for (auto tracker : trackers)
				{
					tracker.second->add(control->getId());
				}
			}

			std::string UIControllerImpl::getConnectionName() const
			{
				return connectionName;
			}

			std::string UIControllerImpl::getUx()
			{
				return std::string{};
			}

			void UIControllerImpl::send(Message msg)
			{
				service::SendToMessage sendto(provider.sendToId, connectionId, msg);
				socket->sendBytes(service::SendtoSerializer::serialize(std::move(sendto)));
			}


			UIControllerImpl::~UIControllerImpl()
			{
				controls.clear();
				checkingTrackers = false;
				if (checkTrackersThread)
				{
					checkTrackersThread->join();
				}
			}

			IdProvider& UIControllerImpl::getIdProvider()
			{
				return provider;
			}

			uint32_t UIControllerImpl::getConnectionId()
			{
				return connectionId;
			}

			ControlHandle UIControllerImpl::getControl(const uint32_t id)
			{
				std::lock_guard<std::mutex> lock(controlMutex);
				auto it = controls.find(id);
				if (it == controls.end())
				{
					return nullptr;
				}
				return  it->second;
			}

			bool UIControllerImpl::hasUpdate(const std::string& tracker)
			{
				std::lock_guard<std::mutex> lock(controlMutex);
				auto it = trackers.find(tracker);
				if (it == trackers.end())
				{
					return false;
				}
				bool update = it->second->hasUpdates();
				it->second->setAccessed();
				return update;
			}

			void UIControllerImpl::clear()
			{
				std::lock_guard<std::mutex> lock(controlMutex);
				controls.clear();
			}

			void UIControllerImpl::notifyUpdate(const uint32_t &controlId)
			{
				std::lock_guard<std::mutex> lock(controlMutex);
				for (auto tracker : trackers)
				{
					tracker.second->setUpdate(controlId, true);
				}
			}

			void UIControllerImpl::addNotification(const NotificationHandle& notification)
			{
				std::lock_guard<std::mutex> lock(notificationMutex);
				const uint32_t id = notification->getId();
				if (notifications.find(id) != notifications.end())
				{
					return;
				}
				notifications.emplace(id, notification);
			}

			NotificationHandle UIControllerImpl::getNotification(const uint32_t id) const
			{
				auto it = notifications.find(id);
				if (it == notifications.end())
				{
					return nullptr;
				}
				return it->second;
			}

			void UIControllerImpl::displayNotification(const uint32_t controlId, const std::string & text, const uint32_t actionId)
			{
				std::lock_guard<std::mutex> lock(notificationMutex);
				auto it = notifications.find(controlId);
				if (it == notifications.end())
				{
					return;
				}
				NotificationData data = { text,controlId };
				pendingNotifications.emplace(actionId, data);
				for (auto tracker : notifTrackers)
				{
					tracker.second->add(actionId);
				}

			}

			bool UIControllerImpl::hasNotifications(const std::string & tracker)
			{
				std::lock_guard<std::mutex> lock(notificationMutex);
				auto it = notifTrackers.find(tracker);
				if (it == notifTrackers.end())
				{
					return false;
				}
				it->second->setAccessed();
				return  it->second->hasUpdates();
			}

			UpdateMap UIControllerImpl::getNotifications(const std::string & tracker, const bool force)
			{
				UpdateMap updates;
				std::lock_guard<std::mutex> lock(notificationMutex);

				auto trackerIt = notifTrackers.find(tracker);
				if (trackerIt == notifTrackers.end())
				{
					return updates;
				}

				if (!force && !trackerIt->second->hasUpdates())
				{
					return updates;
				}

				for (auto notification : pendingNotifications)
				{
					auto it = notifications.find(notification.second.controlId);
					if (it == notifications.end())
					{
						continue;
					}
					if (!force && !trackerIt->second->hasUpdate(notification.first))
					{
						continue;
					}
					std::string notifString = it->second->getUx(notification.second.text, notification.first);
					updates.emplace_back(std::make_pair(getControlId(notification.first, "notif"), notifString));
					trackerIt->second->setUpdate(notification.first, false);
				}
				return updates;
			}

			void UIControllerImpl::dismissNotification(const std::string & tracker, const uint32_t actiondId)
			{
				std::lock_guard<std::mutex> lock(notificationMutex);
				auto it = pendingNotifications.find(actiondId);
				if (it == pendingNotifications.end())
				{
					return;
				}
				pendingNotifications.erase(it);
				for (auto trackerIt : notifTrackers)
				{
					trackerIt.second->remove(actiondId);
				}
			}

			void UIControllerImpl::requestState()
			{
				Message msg = RequestControllerStateSerializer::serialize(std::move(RequestControllerState(provider.requestStateId)));
				send(msg);
			}

			TemplateEngineHandle UIControllerImpl::getEngine()
			{
				return engine;
			}

			void UIControllerImpl::addTracker(const std::string& addr)
			{
				std::lock_guard<std::mutex> lock(controlMutex);
				if (trackers.find(name) != trackers.end())
				{
					return;
				}
				UpdateTrackerHandle tracker = std::make_shared<UpdateTracker>();

				tracker->name = addr;
				for (auto control : controls)
				{
					tracker->add(control.first);
				}
				trackers.emplace(addr, tracker);

				UpdateTrackerHandle notification = std::make_shared<UpdateTracker>();
				notification->name = addr;

				for (auto notif : pendingNotifications)
				{
					tracker->add(notif.first);
				}
				notifTrackers.emplace(addr, notification);

			}

			std::string UIControllerImpl::getControlId(const uint32_t control, const std::string & controlType) const
			{
				std::stringstream stream;
				stream << getConnectionName();
				if (!controlType.empty())
				{
					stream << "-" << controlType;
				}
				stream << "-" << control;

				return stream.str();
			}

			void UIControllerImpl::notifyTrackerUpdate(const std::string& tracker, const uint32_t control)
			{
				std::lock_guard<std::mutex> lock(controlMutex);
				for (auto trackerIt : trackers)
				{
					if (trackerIt.first != tracker)
					{
						trackerIt.second->setUpdate(control, true);
					}
				}
			}

			Removals UIControllerImpl::getRemovals(const std::string& tracker)
			{
				Removals removals; {
					std::lock_guard<std::mutex> lock(controlMutex);

					auto trackerIt = trackers.find(tracker);
					if (trackerIt != trackers.end())
					{
						const auto & removalIds = trackerIt->second->getRemoved();
						for (const auto & id : removalIds)
						{
							removals.emplace_back(getControlId(id));
						}
						trackerIt->second->clearRemoved();
					}
				}
				{
					std::lock_guard<std::mutex> lock(notificationMutex);
					auto notifIt = notifTrackers.find(tracker);
					if (notifIt != notifTrackers.end())
					{
						const auto & removalIds = notifIt->second->getRemoved();
						for (const auto & id : removalIds)
						{
							removals.emplace_back(getControlId(id, "notif"));
						}
						notifIt->second->clearRemoved();
					}
				}
				return removals;

			}

			UpdateMap UIControllerImpl::getUpdates(const std::string& addr, bool force)
			{
				std::lock_guard<std::mutex> lock(controlMutex);

				auto trackerIt = trackers.find(addr);

				UpdateMap updates;
				if (trackerIt == trackers.end())
				{
					return updates;
				}
				if(force)
				{
					updates.emplace_back(std::make_pair("controller_name",generator->generateText(connectionName)));
				}

				if (!force && !trackerIt->second->hasUpdates())
				{
					return updates;
				}

				for (auto control : controls)
				{
					if (!force && !trackerIt->second->hasUpdate(control.first))
					{
						continue;
					}
					updates.emplace_back(std::make_pair(getControlId(control.first), control.second->getUx()));
					trackerIt->second->setUpdate(control.first, false);
				}
				return updates;
			}

			void UIControllerImpl::startCheckingTrackers()
			{
				if (checkingTrackers)
				{
					return;
				}
				checkingTrackers = true;
				checkTrackersThread = std::make_shared<std::thread>([this]()
				{
					uint32_t times = 0;
					while (checkingTrackers) {
						++times;
						if (times == 15) {
							times = 0;
							this->checkTrackers();
						}
						std::this_thread::sleep_for(std::chrono::milliseconds(2000));
					}
				});
			}

			ControlStateHandler::ControlStateHandler(const UIControllerHandle& controller_)
				: controller{ controller_ }
			{
			}

			void ControlStateHandler::handle(messages::MessageBase&& data)
			{
				endpoint::ToggleControlEnabledState & msg = static_cast<endpoint::ToggleControlEnabledState&>(data);

				ControlHandle control = controller->getControl(msg.prop);
				if (!control)
				{
					return;
				}
				control->setState(msg.prop2);
				controller->notifyUpdate(msg.prop);
			}

			ControlShownHandler::ControlShownHandler(const UIControllerHandle& controller_)
				: controller{ controller_ }
			{
			}

			void ControlShownHandler::handle(messages::MessageBase&& data)
			{
				endpoint::ToggleControlShownState & msg = static_cast<endpoint::ToggleControlShownState&>(data);

				ControlHandle control = controller->getControl(msg.prop);
				if (!control)
				{
					return;
				}
				control->setDisplayState(msg.prop2);
				controller->notifyUpdate(msg.prop);
			}

			void UIControllerImpl::checkTrackers()
			{
				std::lock_guard<std::mutex> lock(controlMutex);
				for (auto it = trackers.begin(); it != trackers.end();)
				{
					uint32_t timeSince = it->second->getTimeSinceLastAccess();
					if (timeSince > 30000)
					{
						it = trackers.erase(it);
					}
					else
					{
						++it;
					}
				}
			}
		}
	}
}
