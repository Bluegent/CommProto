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

			UIControllerImpl::UIControllerImpl(const std::string& name, const std::string& connectionName_, const messages::TypeMapperHandle & mapper, const sockets::SocketHandle & socket_, const uint32_t id)
				: UIController{ name }
				, connectionName{ connectionName_ }
				, provider{ mapper }
				, socket{ socket_ }
				, connectionId{ id }
				, update{ true }
				, hasNotif{false}
			{
			}

			void UIControllerImpl::addControl(const ControlHandle& control)
			{
				LOG_INFO("Adding a control named \"%s\"", control->getName().c_str());
				std::lock_guard<std::mutex> lock(controlMutex);
				update = true;
				const uint32_t id = control->getId();
				if (controls.find(id) != controls.end())
				{
					return;
				}
				controls.emplace(id, control);
			}

			std::string UIControllerImpl::getConnectionName()
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

			bool UIControllerImpl::hasUpdate()
			{
				return update;
			}

			void UIControllerImpl::clear()
			{
				std::lock_guard<std::mutex> lock(controlMutex);
				controls.clear();
			}

			void UIControllerImpl::notifyUpdate()
			{
				update = true;
			}

			void UIControllerImpl::addNotification(const NotificationHandle& notification)
			{
				const uint32_t id = notification->getId();		
				if (notifications.find(id) != notifications.end())
				{
					return;
				}
				LOG_INFO("Adding notification with id %d", id);
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
				for(auto id : pendingNotifications)
				{
					auto it = notifications.find(id);
					if(it == notifications.end())
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
				auto it = std::find(pendingNotifications.begin(), pendingNotifications.end(),id);
				if(it == pendingNotifications.end())
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
		}
	}
}
