#ifndef ENDPOINT_CONTROL_ID_PROVIDER_H
#define ENDPOINT_CONTROL_ID_PROVIDER_H

#include <commproto/messages/TypeMapper.h>
#include <commproto/control/ButtonChains.h>
#include <commproto/control/SliderChains.h>
#include <commproto/control/NotificationChains.h>
#include <commproto/control/ToggleChains.h>
#include <commproto/control/LabelChains.h>
#include <commproto/control/ProgressBarChains.h>
#include <commproto/control/ControllerChains.h>
#include <commproto/logger/Logging.h>

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
					, sliderId{ mapper->registerType<SliderMessage>() }
					, progressBarId{ mapper->registerType<ProgressBarMessage>() }
					, progressBarUpdateId{ mapper->registerType<ProgressBarUpdate>() }
					, notificationId{ mapper->registerType<NotificationMessage>() }
					, displayNotificationId{ mapper->registerType<DisplayNotification>() }
					, toggleControlStateId{ mapper->registerType<ToggleControlEnabledState>() }
					, toggleControlShownStateId{ mapper->registerType<ToggleControlShownState>() }
				{
					LOG_DEBUG("Slider id: %d",sliderId);
				}
				const uint32_t buttonId;
				const uint32_t toggleId;
				const uint32_t labelId;
				const uint32_t labelUpdateId;
				const uint32_t sliderId;
				const uint32_t progressBarId;
				const uint32_t progressBarUpdateId;
				const uint32_t notificationId;
				const uint32_t displayNotificationId;
				const uint32_t toggleControlStateId;
				const uint32_t toggleControlShownStateId;
				
			};

		}

	}
}

#endif // CONTROL_ID_PROVIDER_H