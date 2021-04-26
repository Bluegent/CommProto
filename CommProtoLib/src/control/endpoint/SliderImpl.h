#ifndef ENDPOINT_SLIDER_IMPL_H
#define ENDPOINT_SLIDER_IMPL_H
#include <commproto/control/endpoint/Slider.h>
#include <commproto/parser/Handler.h>
#include <commproto/control/endpoint/UIController.h>
#include <commproto/control/SliderChains.h>

namespace commproto
{
	namespace control
	{
		namespace endpoint
		{

			class SliderImpl : public Slider
			{
			public:
				SliderImpl(const std::string& name, uint32_t id, const uint32_t sliderId_, const SliderAction& action_, const std::string & unit_ = {});

				Message serialize() const override;
				float getValue() const override;
				void setValue(const float value_) override;
				void setLimits(const float left_, const float right_) override;
				void setInitialValue(const float value_) override;
				void setStep(const float step_) override;
			private:
				float left, right;
				float value;
				float step;
				const uint32_t sliderId;
				SliderAction action;
				std::string unit;

			};


			class SliderAdjustHandler : public parser::Handler
			{
			public:
				SliderAdjustHandler(const UIControllerHandle& controller_);
				void handle(messages::MessageBase&& data) override;
			private:
				UIControllerHandle controller;
			};
		}
	}
}


#endif // ENDPOINT_SLIDER_IMPL_H