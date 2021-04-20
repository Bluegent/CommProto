#include "UIControllerImpl.h"
#include <sstream>
#include <commproto/service/ServiceChains.h>
#include <commproto/logger/Logging.h>
#include <algorithm>

namespace commproto
{
	namespace control {
		namespace endpoint {
			UIControllerImpl::UIControllerImpl(const std::string& name, const messages::TypeMapperHandle & mapper, const sockets::SocketHandle & socket_)
				: UIController{ name }
				, provider(mapper)
				, idCounter{ 1 } // id 0 is reserved for the ui controller itself
				, socket{ socket_ }
			{
			}

			void UIControllerImpl::addControl(const ControlHandle& control)
			{
				LOG_INFO("Adding a control named \"%s\"", control->getName().c_str());
				const uint32_t id = control->getId();
				if (controls.find(id) != controls.end())
				{
					return;
				}
				controls.emplace(id, control);
			}

			Message UIControllerImpl::serialize() const
			{
				Message result;
				for (auto it = controls.begin(); it != controls.end(); ++it)
				{
					Message controlSerialized = it->second->serialize();
					result.insert(result.end(), controlSerialized.begin(), controlSerialized.end());
					if(!it->second->isEnabled())
					{
						Message state = ToggleControlEnabledStateSerializer::serialize(ToggleControlEnabledState(provider.toggleControlStateId, it->first, false));
						result.insert(result.end(), state.begin(), state.end());
					}
				}


				for (auto it = notifications.begin(); it != notifications.end(); ++it)
				{
					Message controlSerialized = it->second->serialize();
					result.insert(result.end(), controlSerialized.begin(), controlSerialized.end());
				}
				return result;
			}

			UIControllerImpl::~UIControllerImpl()
			{
				controls.clear();
			}

			IdProvider& UIControllerImpl::getIdProvider()
			{
				return provider;
			}

			ControlHandle UIControllerImpl::getControl(const uint32_t id)
			{
				auto it = controls.find(id);
				if (it == controls.end())
				{
					return nullptr;
				}
				return  it->second;
			}

			uint32_t UIControllerImpl::reserveId()
			{
				return idCounter++;
			}

			void UIControllerImpl::clear()
			{
				controls.clear();
			}

			void UIControllerImpl::send(Message msg)
			{
				if (!socket)
				{
					return;
				}
				socket->sendBytes(msg);
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

			void UIControllerImpl::displayNotification(const uint32_t id) const
			{
				auto it = notifications.find(id);
				if (it == notifications.end())
				{
					return;
				}
				socket->sendBytes(it->second->serializeDisplay());
			}

			void UIControllerImpl::sendState()
			{
				send(serialize());
			}

			void UIControllerImpl::setControlState(const uint32_t id, const bool enabled)
			{
				ControlHandle control = getControl(id);
				if (!control)
				{
					return;
				}
				control->setState(enabled);
				Message state = ToggleControlEnabledStateSerializer::serialize(ToggleControlEnabledState(provider.toggleControlStateId, id, enabled));
				send(state);
			}

			void UIControllerImpl::setControlShownState(const uint32_t id, const bool shown)
			{
				ControlHandle control = getControl(id);
				if(!control)
				{
					return;
				}
				control->setDisplayState(shown);
				Message state = ToggleControlShownStateSerializer::serialize(ToggleControlShownState(provider.toggleControlShownStateId, id, shown));
				send(state);
			}

			ControlHandle UIControllerImpl::getControlFromAll(const uint32_t id) const
			{
				auto it = controls.find(id);
				if (it != controls.end())
				{
					it->second;
				}
				auto notif = notifications.find(id);
				if (notif != notifications.end())
				{
					return notif->second;
				}
				return ControlHandle();
			}

			RequestStateHandler::RequestStateHandler(const UIControllerHandle& controller_)
				:controller{ controller_ }
			{
			}

			void RequestStateHandler::handle(messages::MessageBase&& data)
			{
				LOG_INFO("Got a request for ui state");
				controller->sendState();
			}
		}
	}
}
