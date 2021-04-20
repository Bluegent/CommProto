#include "UIControllerImpl.h"
#include <sstream>
#include <commproto/logger/Logging.h>
#include <algorithm>
#include <commproto/control/ControllerChains.h>

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
				, update{ true }
				, hasNotif{ false }
				, engine{ engine_ }
				, checkingTrackers(false)
			{
			}

			void UIControllerImpl::addControl(const ControlHandle& control)
			{
				std::lock_guard<std::mutex> lock(controlMutex);
				update = true;
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
				std::stringstream stream;
				std::lock_guard<std::mutex> lock(controlMutex);
				for (auto it = controls.begin(); it != controls.end(); ++it)
				{
					stream << it->second->getUx();
				}
				update = false;
				return stream.str();
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
				if(checkTrackersThread)
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

			void UIControllerImpl::displayNotification(const uint32_t id)
			{
				auto it = notifications.find(id);
				if (it == notifications.end())
				{
					return;
				}
				std::lock_guard<std::mutex> lock(notificationMutex);
				pendingNotifications.push_back(id);
				hasNotif = true;

			}

			bool UIControllerImpl::hasNotifications()
			{
				return hasNotif;
			}

			std::string UIControllerImpl::getNotifications()
			{
				std::stringstream stream;
				std::lock_guard<std::mutex> lock(notificationMutex);
				for (auto id : pendingNotifications)
				{
					auto it = notifications.find(id);
					if (it == notifications.end())
					{
						continue;
					}
					stream << it->second->getUx();
				}
				hasNotif = false;
				return stream.str();
			}

			void UIControllerImpl::dismissNotification(const uint32_t id)
			{
				std::lock_guard<std::mutex> lock(notificationMutex);
				auto it = std::find(pendingNotifications.begin(), pendingNotifications.end(), id);
				if (it == pendingNotifications.end())
				{
					return;
				}

				pendingNotifications.erase(it);
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

			}

			std::string UIControllerImpl::getControlId(const uint32_t control) const
			{
				std::stringstream stream;
				stream << getConnectionName() << "-" << control;
				return stream.str();
			}

			std::map<std::string, std::string> UIControllerImpl::getUpdates(const std::string& addr, bool force)
			{
				std::lock_guard<std::mutex> lock(controlMutex);
				
				auto trackerIt = trackers.find(addr);

				std::map<std::string, std::string> updates;
				if (trackerIt == trackers.end())
				{
					return updates;
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
					updates.emplace(getControlId(control.first), control.second->getUx());
					trackerIt->second->setUpdate(control.first, false);
				}
				return updates;
			}

			void UIControllerImpl::startCheckingTrackers()
			{
				if(checkingTrackers)
				{
					return;
				}
				checkingTrackers = true;
				checkTrackersThread = std::make_shared<std::thread>([this]() 
				{
					uint32_t times = 0;
					while (checkingTrackers) {
						++times;
						if (times == 3) {
							times = 0;
							this->checkTrackers();
						}
						std::this_thread::sleep_for(std::chrono::milliseconds(1000));
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
