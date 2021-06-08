#ifndef BASE_CONTROL_TYPE_H
#define BASE_CONTROL_TYPE_H
#include <cstdint>

namespace commproto
{
	namespace control
	{
		namespace ux
		{
			enum class BaseControlType : uint32_t
			{
				Button,
				Label,
				ProgressBar,
				Slider,
				Toggle,
				Notification,
				Unknown,
			};
        }
    }
}
#endif