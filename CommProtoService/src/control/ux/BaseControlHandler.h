#ifndef BASE_CONTROL_HANDLER_H
#define BASE_CONTROL_HANDLER_H

#include <commproto/control/ux/ControlHandler.h>
namespace commproto
{
	namespace control
	{
		namespace ux
		{
			enum class ControlType : uint8_t
			{
				Button = 0,
				Slider,
				Toggle,
				Label,
				Notification
			};

			class BaseControlHandler : public ControlHandler
			{
			public:
				void handle(AttributeMap&& attributes, UIController& controller) override;
			private:
				void handleButton(AttributeMap&& attributes, UIController& controller) const;
				void handleSlider(AttributeMap&& attributes, UIController& controller) const;
				void handleToggle(AttributeMap&& attributes, UIController& controller) const;
				void handleNotification(AttributeMap&& attributes, UIController& controller) const;
			};
		}
	}
}
#endif // BASE_CONTROL_HANDLER_H