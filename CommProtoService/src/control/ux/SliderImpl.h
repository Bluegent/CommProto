#ifndef UX_SLIDER_IMPL_H
#define UX_SLIDER_IMPL_H
#include <commproto/control/ux/Slider.h>
#include <commproto/control/SliderChains.h>
#include <commproto/parser/Handler.h>
#include <commproto/control/ux/UIController.h>
#include "BaseControlType.h"

namespace commproto
{
	namespace control
	{
		namespace ux
		{
			class SliderImpl : public Slider
			{
			public:
				SliderImpl(const std::string& name, uint32_t id, const uint32_t sliderAdjustId_, const GeneratorHandle & generator_);

				UxContainerHandle getUx() override;
				void setValue(const float) override;
				void setValues(const float left_, const float right_, const float value_, const float step_) override;
				void getValues(float & left_, float & right_, float & value_, float & step_) const;
				std::string getUnitOfMeasure() const override { return unitOfMeasure; }
				void setUnitOfMeasure(const std::string& unit) override { unitOfMeasure = unit; }
				uint32_t getType() const override { return static_cast<uint32_t>(BaseControlType::Slider); }
			private:
				float left, right;
				float value;
				float step;
				const uint32_t sliderAdjustId;
				GeneratorHandle generator;
				std::string unitOfMeasure;
			};



			class SliderHandler : public parser::Handler
			{
			public:
				SliderHandler(const UIControllerHandle& controller_);
				void handle(messages::MessageBase&& data) override;
			private:
				UIControllerHandle controller;
			};
		}
	}
}


#endif //UX_SLIDER_IMPL_H