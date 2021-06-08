#ifndef UX_BUTTON_IMPL_H
#define Ux_BUTTON_IMPL_H

#include <commproto/control/ux/Button.h>
#include <commproto/control/ux/UIController.h>
#include <commproto/control/ButtonChains.h>
#include <commproto/control/ux/UxGenerator.h>
#include "BaseControlType.h"

namespace commproto
{
	namespace control
	{
		namespace ux
		{
			class ButtonHandler : public parser::Handler
			{
			public:
				ButtonHandler(const UIControllerHandle& controller_);
				void handle(messages::MessageBase&& data) override;
			private:
				UIControllerHandle controller;

			};

			class ButtonImpl : public Button
			{
			public:
				ButtonImpl(const std::string& name_, const uint32_t id, const uint32_t pressId, const GeneratorHandle& generator_);

				void press() override;

				UxContainerHandle getUx() override;
				uint32_t getType() const override { return static_cast<uint32_t>(BaseControlType::Button); }
			private:
				GeneratorHandle generator;
				const uint32_t pressId;
			};
		}

	}
}

#endif // UX_BUTTON_IMPL_H