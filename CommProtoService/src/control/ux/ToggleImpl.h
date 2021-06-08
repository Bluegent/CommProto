#ifndef UX_TOGGLE_IMPL_H
#define UX_TOGGLE_IMPL_H

#include <commproto/control/ux/Toggle.h>
#include <commproto/control/ux/UIController.h>
#include <commproto/control/ToggleChains.h>
#include "BaseControlType.h"

namespace commproto
{
    namespace control
	{

		namespace ux
		{

			class ToggleHandler : public parser::Handler
			{
			public:
				ToggleHandler(const ux::UIControllerHandle& controller_);
				void handle(messages::MessageBase&& data) override;
			private:
				ux::UIControllerHandle controller;
			};

			class ToggleImpl : public Toggle {
			public:
				ToggleImpl(const std::string& name, uint32_t id, const uint32_t messageId, const GeneratorHandle& generator_, const bool defaultState = false);
				UxContainerHandle getUx() override;
				void toggle() override;
				bool getState() const override;
				uint32_t getType() const override { return static_cast<uint32_t>(BaseControlType::Toggle); }
			private:
				GeneratorHandle generator;
				const uint32_t messageId;
				bool state;
			};
		}
    }
}


#endif // UX_TOGGLE_IMPL_H