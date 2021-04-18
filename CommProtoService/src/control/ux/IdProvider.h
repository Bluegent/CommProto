#ifndef UX_CONTROL_ID_PROVIDER_H
#define UX_CONTROL_ID_PROVIDER_H
#include <commproto/service/ServiceChains.h>
#include <commproto/messages/TypeMapper.h>
#include <commproto/control/ButtonChains.h>
#include <commproto/control/ToggleChains.h>
#include <commproto/control/NotificationChains.h>
#include <commproto/control/ControllerChains.h>
#include <commproto/control/SliderChains.h>

namespace commproto
{
	namespace control
	{
		namespace ux
		{
			class IdProvider
			{
			public:
				IdProvider(const messages::TypeMapperHandle& mapper)
					: buttonId{ mapper->registerType<PressButtonMessage>() }
					, toggleId{ mapper->registerType<ToggleMessage>() }
					, sendToId{ mapper->registerType<service::SendToMessage>() }
					, notificationResponseId{ mapper->registerType<NotificationResponseMessage>() }
					, requestStateId{ mapper->registerType<RequestControllerState>() }
					, adjustSliderId{ mapper->registerType<SliderAdjust>() }
				
				{

				}
				const uint32_t buttonId;
				const uint32_t toggleId;
				const uint32_t sendToId;
				const uint32_t notificationResponseId;
				const uint32_t requestStateId;
				const uint32_t adjustSliderId;
			};
		}

	}
}

#endif // CONTROL_ID_PROVIDER_H