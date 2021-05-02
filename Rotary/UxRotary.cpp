#include "UxRotary.h"
#include <commproto/logger/Logging.h>
#include <commproto/utils/String.h>
#include <sstream>


namespace rotary
{
	namespace ux
	{
		Rotary::Rotary(const std::string& name, const uint32_t id, const uint32_t rotaryMoveId_, const control::ux::UIControllerHandle & controller_)
			: Control(name, id)
			, rotaryMoveId{ rotaryMoveId_ }
			, value{ 0.f }
			, left{ 0.f }
			, right{ 1.f }
			, step(0.1f)
			, controller{ controller_ }
		{
		}

		void Rotary::setValue(const float value_)
		{
			if (value< left || value > right)
			{
				return;
			}
			value = value_;
			if (!controller)
			{
				return;
			}
			commproto::Message msg = RotaryAdjustSerializer::serialize(RotaryAdjust(rotaryMoveId, id, value));
			controller->send(msg);
		}

		float Rotary::getValue() const
		{
			return value;
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

		std::string getTemplate()
		{
			//TODO: make template
			std::stringstream stream;
			return stream.str();
		}

		std::string Rotary::getUx()
		{
			std::string rotaryTemplate = getTemplate();
			utils::replaceAll(rotaryTemplate, "@id", utils::getString(id));
			utils::replaceAll(rotaryTemplate, "@name", name);
			utils::replaceAll(rotaryTemplate, "@rotary_id", controller->getControlId(id, "rotary"));
			utils::replaceAll(rotaryTemplate, "@left", utils::getString(left));
			utils::replaceAll(rotaryTemplate, "@right", utils::getString(right));
			utils::replaceAll(rotaryTemplate, "@value", utils::getString(value));
			utils::replaceAll(rotaryTemplate, "@step", utils::getString(step));
			return rotaryTemplate;

		}

		RotaryHandler::RotaryHandler(const control::ux::UIControllerHandle& controller_)
			: controller{ controller_ }
		{
		}

		void RotaryHandler::handle(messages::MessageBase&& data)
		{
			endpoint::RotaryMessage & msg = static_cast<endpoint::RotaryMessage&>(data);

			if (msg.prop2.size() != 2)
			{
				LOG_ERROR("Insufficient rotary parameters for rotary \"%d\"", msg.prop);
				return;
			}

			if (msg.prop3.size() != 4)
			{
				LOG_ERROR("Insufficient rotary parameters for rotary \"%s\"", msg.prop2[0].c_str());
				return;
			}
			float left = msg.prop3[0];
			float right = msg.prop3[1];
			float value = msg.prop3[2];
			float step = msg.prop3[3];

			RotaryHandle rotary = std::make_shared<Rotary>(msg.prop2[0], msg.prop, controller->getMapper()->registerType<RotaryAdjust>(), controller);
			controller->addControl(rotary);
		}
	}
}
