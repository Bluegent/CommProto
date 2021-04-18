#ifndef ENDPOINT_CONTROL_ID_PROVIDER_H
#define ENDPOINT_CONTROL_ID_PROVIDER_H

#include <commproto/messages/TypeMapper.h>
#include <commproto/control/ButtonChains.h>
#include <commproto/control/SliderChains.h>
#include <commproto/control/NotificationChains.h>
#include <commproto/control/ToggleChains.h>
#include <commproto/control/LabelChains.h>
#include <commproto/control/ControllerChains.h>

namespace commproto
{
	namespace control
	{
		namespace endpoint
		{
			class IdProvider
			{
			public:
				IdProvider(const messages::TypeMapperHandle& mapper)
					: buttonId{ mapper->registerType<ButtonMessage>() }
					, toggleId{ mapper->registerType<ToggleMessage>() }
					, labelId{ mapper->registerType<LabelMessage>() }
					, labelUpdateId{ mapper->registerType<LabelUpdateMessage>() }
					, notificationId{ mapper->registerType<NotificationMessage>() }
					, displayNotificationId{ mapper->registerType<DisplayNotificationMessage>() }
					, toggleControlStateId{ mapper->registerType<ToggleControlEnabledState>() }
					, toggleControlShownStateId{ mapper->registerType<ToggleControlShownState>() }
				{

				}
				const uint32_t buttonId;
				const uint32_t toggleId;
				const uint32_t labelId;
				const uint32_t labelUpdateId;
				const uint32_t notificationId;
				const uint32_t displayNotificationId;
				const uint32_t toggleControlStateId;
				const uint32_t toggleControlShownStateId;
			};

		}

	}
}

#endif // CONTROL_ID_PROVIDER_H