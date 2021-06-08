#ifndef UX_CONTROL_H
#define UX_CONTROL_H

#include <commproto/common/Common.h>
#include <memory>
#include <commproto/control/BaseControl.h>

namespace commproto
{
	namespace control {

		namespace ux
		{			
			class UxContainer
			{
			public:
				virtual ~UxContainer() = default;
			};
			using UxContainerHandle = std::shared_ptr<UxContainer>;

			class Control : public BaseControl {
			public:
				Control(const std::string & name, const uint32_t id) : BaseControl(name,id) {}
				virtual ~Control() = default;
				virtual UxContainerHandle getUx() = 0;
				virtual uint32_t getType() const = 0;
			};
			using ControlHandle = std::shared_ptr<Control>;
		}


	}
}


#endif // UX_CONTROL_H