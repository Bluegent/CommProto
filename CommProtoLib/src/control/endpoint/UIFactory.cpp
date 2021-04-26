#include <commproto/control/endpoint/UIFactory.h>
#include "ButtonImpl.h"
#include "UIControllerImpl.h"
#include "IdProvider.h"
#include "NotificationImpl.h"
#include "LabelImpl.h"
#include "ToggleImpl.h"
#include "SliderImpl.h"
#include "ProgressBarImpl.h"

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

			SliderHandle UIFactory::makeSlider(const std::string& name, const SliderAction& action, const std::string & unitOfMeasure) const
			{
				return std::make_shared<SliderImpl>(name, controller->reserveId(), controller->getIdProvider().sliderId, action,unitOfMeasure);
			}

			ToggleHandle UIFactory::makeToggle(const std::string& name, const ToggleAction& action, const bool defaultState) const
			{
				return std::make_shared<ToggleImpl>(name, controller->reserveId(), controller->getIdProvider().toggleId, action, defaultState);
			}

			LabelHandle UIFactory::makeLabel(const std::string& name, const std::string& text) const
			{
				return std::make_shared<LabelImpl>(name, controller->reserveId(), controller->getIdProvider().labelId, controller->getIdProvider().labelUpdateId, text, controller);
			}

			ProgressBarHandle UIFactory::makeProgresBar(const std::string& name, const uint32_t progress) const
			{
				return std::make_shared<ProgressBarImpl>(name, controller->reserveId(), controller, controller->getIdProvider().progressBarId, controller->getIdProvider().progressBarUpdateId,progress);
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
