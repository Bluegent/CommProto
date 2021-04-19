#include "SliderImpl.h"
#include <commproto/logger/Logging.h>


namespace commproto
{
	namespace control
	{
		namespace endpoint
		{
			SliderImpl::SliderImpl(const std::string& name, uint32_t id, const uint32_t sliderId_, const SliderAction& action_)
				: Slider(name, id)
				, left{ 0.f }
				, right{ 1.f }
				, value{ 0.5f }
				, step{ 0.f }
				, sliderId{ sliderId_ }
				, action{ action_ }
			{
			}

			Message SliderImpl::serialize() const
			{
				std::vector<float> props;
				props.push_back(left);
				props.push_back(right);
				props.push_back(value);
				props.push_back(step);
				return SliderMessageSerializer::serialize(std::move(SliderMessage(sliderId, id, name, props)));
			}

			float SliderImpl::getValue() const
			{
				return value;
			}

			void SliderImpl::setValue(const float value_)
			{
				if (value < left || value > right)
				{
					return;
				}
				value = value_;
				action(value);
			}

			void SliderImpl::setLimits(const float left_, const float right_)
			{
				if (right_ <= left_)
				{
					return;
				}
				right = right_;
				left = left_;
			}

			void SliderImpl::setInitialValue(const float value_)
			{
				if (value<0.f || value > 1.f)
				{
					return;
				}
				value = value_;
			}

			void SliderImpl::setStep(const float step_)
			{
				if (step > (right - left)/2.f || step < 0.f)
				{
					return;
				}
				step = step_;
			}

			SliderAdjustHandler::SliderAdjustHandler(const UIControllerHandle& controller_)
				: controller{ controller_ }
			{
			}

			void SliderAdjustHandler::handle(messages::MessageBase&& data)
			{
				ux::SliderAdjust& msg = static_cast<ux::SliderAdjust&>(data);
				ControlHandle slider = controller->getControl(msg.prop);

				if (!slider)
				{
					return;
				}

				SliderHandle sliderCast = std::static_pointer_cast<SliderImpl>(slider);

				if (!sliderCast)
				{
					return;
				}

				sliderCast->setValue(msg.prop2);
			}
		}
	}
}
