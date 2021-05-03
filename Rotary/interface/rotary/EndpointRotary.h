#ifndef ENDPOINT_ROTARY_H
#define ENDPOINT_ROTARY_H

#include <commproto/control/endpoint/Control.h>
#include "RotaryChains.h"
#include <functional>
#include <commproto/control/endpoint/UIController.h>
#include <rotary/RotaryChains.h>
#include <commproto/plugin/Definitions.h>

using namespace commproto;

namespace rotary
{
	namespace endpoint
	{
		using RotaryAction = std::function<void(const float)>;
		class DLL_EXPORT_SYMBOL Rotary : public control::endpoint::Control
		{
		public:
			Rotary(const std::string& name, const uint32_t id, const uint32_t rotaryId, const RotaryAction& action_);

			void setValues(const float left_, const float right_, const float step_);
			void setValue(const float value_);
			float getValue() const;
			Message serialize() const override;
		private:
			const uint32_t rotaryId;
			RotaryAction action;
			float value;
			float left, right;
			float step;
		};

		using RotaryHandle = std::shared_ptr<Rotary>;


		class DLL_EXPORT_SYMBOL RotaryHandler : public parser::Handler
		{
		public:
			RotaryHandler(const control::endpoint::UIControllerHandle & controller);
			void handle(messages::MessageBase&& data) override;
		private:
			control::endpoint::UIControllerHandle controller;
		};

	}
}

#endif // ENDPOINT_ROTARY_H
