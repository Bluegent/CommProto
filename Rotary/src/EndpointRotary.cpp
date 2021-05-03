#include <rotary/EndpointRotary.h>
#include <commproto/logger/Logging.h>


namespace rotary
{
	namespace endpoint
	{
		Rotary::Rotary(const std::string& name, const uint32_t id, const uint32_t rotaryId_, const RotaryAction& action_)
			: Control(name, id)
			, rotaryId{ rotaryId_ }
			, action{ action_ }
			, value{ 0.f }
			, left{ 0.f }
			, right{ 1.f }
			, step{ 0.1f }
		{
		}

		void Rotary::setValues(const float left_, const float right_, const float step_)
		{
			if (left >= right)
			{
				return;
			}
			left = left_;
			right = right_;
			step = step_;
		}

		void Rotary::setValue(const float value_)
		{
			if (value_ < left || value_ > right)
			{
				return;
			}
			value = value_;
			action(value);

		}

		float Rotary::getValue() const
		{
			return value;
		}

		Message Rotary::serialize() const
		{
			std::vector<std::string> props;
			props.push_back(name);
			std::vector<float> floatProps;
			floatProps.push_back(left);
			floatProps.push_back(right);
			floatProps.push_back(value);
			floatProps.push_back(step);

			return RotaryMessageSerializer::serialize(RotaryMessage(rotaryId, id, props, floatProps));
		}

		RotaryHandler::RotaryHandler(const control::endpoint::UIControllerHandle& controller_)
			:controller{ controller_ }
		{
		}

		void RotaryHandler::handle(messages::MessageBase&& data)
		{
			if (!controller)
			{
				return;
			}
			ux::RotaryAdjust & msg = static_cast<ux::RotaryAdjust&>(data);

			auto control = controller->getControl(msg.prop);
			if (!control)
			{
				LOG_ERROR("Could not retrieve a rotary control for id %d", msg.prop);
			}

			auto cast = std::static_pointer_cast<Rotary>(control);

			if (!cast)
			{
				LOG_ERROR("Could not cast a rotary control for id %d", msg.prop);
			}
			cast->setValue(msg.prop2);
		}
	}
}
