#ifndef UX_CONTROL_COLLECTION_H
#define UX_CONTROL_COLLECTION_H
#include <commproto/control/ux/Control.h>

namespace commproto
{
    namespace control
    {
        namespace ux
        {
            class ControlCollection
            {
                public:
                virtual ~ControlCollection() = default;
				virtual void addControl(const ControlHandle & control) = 0;
				virtual bool hasUpdate(const std::string& tracker) = 0;
				virtual ControlHandle getControl(const uint32_t id) = 0;
				virtual void clear() = 0;
				virtual void notifyUpdate(const uint32_t &controlId) = 0;
			};
        }
    }
}

#endif //UX_CONTROL_COLLECTION_H