#ifndef ENDPOINT_CONTROL_COLLECTION_H
#define ENDPOINT_CONTROL_COLLECTION_H
#include <commproto/control/endpoint/Control.h>

namespace commproto
{
    namespace control
    {
		namespace endpoint
		{
			class ControlCollection
			{
			public:
				virtual ~ControlCollection() = default;
				virtual void addControl(const ControlHandle & control) = 0;
				virtual uint32_t reserveId() = 0;
				virtual ControlHandle getControl(const uint32_t id) = 0;
				virtual void clear() = 0;
				virtual void setControlState(const uint32_t id, const bool enabled) = 0;
				virtual void setControlShownState(const uint32_t id, const bool shown) = 0;
			};
		}
    }
}

#endif //ENDPOINT_CONTROL_COLLECTION_H