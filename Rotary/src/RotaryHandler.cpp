#include <rotary/RotaryHandler.h>
#include <commproto/control/ux/UIController.h>
#include <rotary/UxRotary.h>
#include <stdexcept>

namespace rotary
{
	namespace ux
	{
		void RotaryControlHandler::handle(commproto::control::ux::AttributeMap&& attributes, commproto::control::ux::UIController& controller)
		{
			commproto::control::ux::ControlData data = commproto::control::ux::ControlHandlerUtil::getBase(attributes);
			if(data.extension != "rotary" || data.controlType != "rotary")
			{
				return;
			}
			if(!data.id)
			{
				return;
			}
			auto control = controller.getControl(data.id);

			if(!control)
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

			RotaryHandle rotary = std::static_pointer_cast<Rotary>(control);
			if(!rotary)
			{
				return;
			}

			rotary->setValue(value);
			controller.notifyTrackerUpdate(data.tracker, data.id);			
		}
	}
}
