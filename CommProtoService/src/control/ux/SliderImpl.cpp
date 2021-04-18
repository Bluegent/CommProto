#include "SliderImpl.h"
#include <commproto/logger/Logging.h>
#include "UxGenerator.h"
#include "IdProvider.h"

namespace commproto
{
	namespace control
	{
		namespace ux
		{
			SliderImpl::SliderImpl(const std::string& name, uint32_t id, const uint32_t sliderAdjustId_, const GeneratorHandle & generator_)
				: Slider(name, id)
				, left{ 0.f }
				, right{ 1.f }
				, value{ 0.5f }
				, step{ 0.f }
				, sliderAdjustId{ sliderAdjustId_ }
				, generator{ generator_ }
			{
			}

			std::string SliderImpl::getUx()
			{
				return generator->generate(*this);
			}

			void SliderImpl::setValue(const float value_)
			{
				value = value_;
				Message msg = SliderAdjustSerializer::serialize(std::move(SliderAdjust(sliderAdjustId, id, value)));
				generator->send(std::move(msg));
			}

			void SliderImpl::getValues(float& left_, float& right_, float& value_, float& step_) const
			{
				left_ = left;
				right_ = right;
				value_ = value;
				step_ = step;
			}

			void SliderImpl::setValues(const float left_, const float right_, const float value_, const float step_)
			{
				left = left_;
				right = right_;
				value = value_;
				step = step_;
			}

			SliderHandler::SliderHandler(const UIControllerHandle& controller_)
				: controller{ controller_ }
			{
			}

			void SliderHandler::handle(messages::MessageBase&& data)
			{
				endpoint::SliderMessage& msg = static_cast<endpoint::SliderMessage&>(data);

				if (msg.prop3.size() != 4)
				{
					LOG_ERROR("Insufficient slider parameters for slider \"%s\"", msg.prop2.c_str());
					return;
				}
				float left = msg.prop3[0];
				float right = msg.prop3[1];
				float value = msg.prop3[2];
				float step = msg.prop3[3];
				SliderHandle slider = std::make_shared<SliderImpl>(msg.prop2, msg.prop, controller->getIdProvider().adjustSliderId, std::make_shared<Generator>(*controller));
				slider->setValues(left, right, value, step);
				controller->addControl(slider);

			}
		}
	}
}
