#ifndef CONTROLLER_CHAINS_H
#define CONTROLLER_CHAINS_H

#include <commproto/messages/DoublePropertyChain.h>
#include <commproto/messages/EmptyChain.h>
#include <commproto/messages/MessageName.h>

namespace commproto
{
	namespace control
	{
		namespace endpoint
		{
			MAKE_DOUBLE_PROP_CHAIN(ToggleControlEnabledState, uint32_t, bool);
			MAKE_DOUBLE_PROP_CHAIN(ToggleControlShownState, uint32_t, bool);
		}
		namespace ux
		{
			MAKE_EMPTY_CHAIN(RequestControllerState);
		}
	}

	DEFINE_DATA_TYPE(control::ux::RequestControllerState);
	DEFINE_DATA_TYPE(control::endpoint::ToggleControlEnabledState);
	DEFINE_DATA_TYPE(control::endpoint::ToggleControlShownState);
}

#endif// LABEL_CHAINS_H
