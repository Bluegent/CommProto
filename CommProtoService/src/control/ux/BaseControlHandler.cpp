#include "BaseControlHandler.h"
#include <commproto/control/ux/Button.h>
#include <commproto/control/ux/UIController.h>
#include <commproto/control/ux/Slider.h>
#include <commproto/control/ux/Toggle.h>
#include <commproto/control/ux/Notification.h>

namespace commproto
{
	namespace control
	{
		namespace ux
		{
			const std::map<std::string, ControlType> stringMap = {
				{ "button",		ControlType::Button },
				{ "slider",		ControlType::Slider },
				{ "toggle",		ControlType::Toggle },
				{ "label",		ControlType::Label },
				{ "slider",		ControlType::Slider },
				{ "notification",ControlType::Notification }
			};


			void BaseControlHandler::handle(AttributeMap&& attributes, UIController& controller)
			{
				auto type = attributes.find("controlType");
				if (type == attributes.end())
				{
					return;
				}
				auto typeValue = stringMap.find(type->second);
				if (typeValue == stringMap.end())
				{
					return;
				}

				switch (typeValue->second)
				{
				case ControlType::Button:
					handleButton(std::move(attributes), controller);
					break;
				case ControlType::Slider:
					handleSlider(std::move(attributes), controller);
					break;
				case ControlType::Toggle:
					handleToggle(std::move(attributes), controller);
					break;
				case ControlType::Label: break;
				case ControlType::Notification:
					handleNotification(std::move(attributes), controller);
					break;
				default:;
				}
			}

			void BaseControlHandler::handleButton(AttributeMap&& attributes, UIController& controller) const
			{
				ControlData data = ControlHandlerUtil::getBase(attributes);
				if (!data.id)
				{
					return;
				}

				ButtonHandle button = std::static_pointer_cast<Button>(controller.getControl(data.id));

				if (!button)
				{
					return;
				}

				button->press();
			}

			void BaseControlHandler::handleSlider(AttributeMap&& attributes, UIController& controller) const
			{
				ControlData data = ControlHandlerUtil::getBase(attributes);
				if (!data.id)
				{
					return;
				}
				auto it = attributes.find("value");
				if (it == attributes.end())
				{
					return;
				}

				float value = 0.f;
				try
				{
					value = std::stof(it->second);
				}
				catch (std::invalid_argument arg)
				{
					return;
				}
				SliderHandle slider = std::static_pointer_cast<Slider>(controller.getControl(data.id));

				if (!slider)
				{
					return;
				}
				controller.notifyTrackerUpdate(data.tracker, data.id);
				slider->setValue(value);
			}

			void BaseControlHandler::handleToggle(AttributeMap&& attributes, UIController& controller) const
			{
				ControlData data = ControlHandlerUtil::getBase(attributes);
				if (!data.id)
				{
					return;
				}
				ToggleHandle toggle = std::static_pointer_cast<Toggle>(controller.getControl(data.id));

				if (!toggle)
				{
					return;
				}
				controller.notifyTrackerUpdate(data.tracker, data.id);
				toggle->toggle();
			}

			void BaseControlHandler::handleNotification(AttributeMap&& attributes, UIController& controller) const
			{
				ControlData data = ControlHandlerUtil::getBase(attributes);
				if (!data.id)
				{
					return;
				}

				auto it = attributes.find("option");
				if (it == attributes.end())
				{
					return;
				}

				auto it2 = attributes.find("actionId");

				if (it2 == attributes.end())
				{
					return;
				}

				uint32_t actionId = -1;

				try
				{
					actionId = std::stoi(it2->second);
				}
				catch (std::invalid_argument arg)
				{
					return;
				}

				NotificationHandle notif = std::static_pointer_cast<Notification>(controller.getNotification(data.id));
				if (!notif)
				{
					return;
				}

				notif->execute(it->second, actionId);
				controller.dismissNotification(data.tracker, actionId);
			}
		}
	}
}
