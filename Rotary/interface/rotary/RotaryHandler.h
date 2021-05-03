#ifndef ROTARY_HANDLER_H
#define ROTARY_HANDLER_H
#include <commproto/control/ux/ControlHandler.h>

namespace rotary
{
	namespace ux
	{
		class RotaryControlHandler : public commproto::control::ux::ControlHandler
		{
		public:
			void handle(commproto::control::ux::AttributeMap&& attributes, commproto::control::ux::UIController& controller) override;
		};


	}
}



#endif// ROTARY_HANDLER_H