#ifndef UX_ROTARY_H
#define UX_ROTARY_H

#include <commproto/control/ux/Control.h>
#include <commproto/control/ux/UIController.h>
#include "RotaryChains.h"
#include <commproto/plugin/Definitions.h>

using namespace commproto;

namespace rotary
{
	namespace ux
	{
		class Rotary : public control::ux::Control
		{
		public:
			Rotary(const std::string& name, const uint32_t id, const uint32_t rotaryMoveId_,const control::ux::UIControllerHandle & controller_);

			void setValues(const float left_, const float right_, const float step_);
			void setValue(const float value_);
			float getValue() const;
			std::string getUx() override;

		private:
			const uint32_t rotaryMoveId;
			float value;
			float left, right;
			float step;
			control::ux::UIControllerHandle controller;
		};

		using RotaryHandle = std::shared_ptr<Rotary>;


		class RotaryHandler : public parser::Handler
		{
		public:
			RotaryHandler(const control::ux::UIControllerHandle & controller);
			void handle(messages::MessageBase&& data) override;
		private:
			control::ux::UIControllerHandle controller;
			uint32_t rotdayAdjustId;
		};

	}
}

#endif // UX_ROTARY_H
