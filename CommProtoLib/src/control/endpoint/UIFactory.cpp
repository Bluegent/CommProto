#include <commproto/control/endpoint/UIFactory.h>
#include "ButtonImpl.h"
#include "UIControllerImpl.h"
#include "IdProvider.h"
#include "NotificationImpl.h"
#include "LabelImpl.h"
#include "ToggleImpl.h"
#include "SliderImpl.h"

namespace commproto
{
	namespace control
	{
		namespace endpoint
		{
			UIFactory::UIFactory(const std::string & name, const messages::TypeMapperHandle& mapper, const sockets::SocketHandle& socket)
				: controller{ std::make_shared<UIControllerImpl>(name, mapper, socket) }
			{
			}

			ButtonHandle UIFactory::makeButton(const std::string& name, const ButtonAction& action) const
			{
				
				ButtonHandle button = std::make_shared<ButtonImpl>(name, controller->reserveId(), controller->getIdProvider().buttonId, action);
				return button;
			}

			SliderHandle UIFactory::makeSlider(const std::string& name, const SliderAction& action) const
			{
				return std::make_shared<SliderImpl>(name, controller->reserveId(), controller->getIdProvider().sliderId, action);
			}

			ToggleHandle UIFactory::makeToggle(const std::string& name, const ToggleAction& action, const bool defaultState) const
			{
				return std::make_shared<ToggleImpl>(name, controller->reserveId(), controller->getIdProvider().toggleId, action, defaultState);
			}

			LabelHandle UIFactory::makeLabel(const std::string& name, const std::string& text) const
			{
				return std::make_shared<LabelImpl>(name, controller->reserveId(), controller->getIdProvider().labelId, controller->getIdProvider().labelUpdateId, text, controller);
			}

			NotificationHandle UIFactory::makeNotification(const std::string& name) const
			{
				return std::make_shared<NotificationImpl>(name, controller->reserveId(), controller->getIdProvider().notificationId,controller->getIdProvider().displayNotificationId);
			}

			UIControllerHandle UIFactory::makeController() const
			{
				return  controller;
			}
		}
	}
}
