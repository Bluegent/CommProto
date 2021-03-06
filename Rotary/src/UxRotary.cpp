#include <rotary/UxRotary.h>
#include <commproto/logger/Logging.h>
#include <commproto/utils/String.h>
#include <sstream>
#include <map>
#include <string>
#include "commproto/control/ux/HtmlUxContainer.h"


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

		control::ux::UxContainerHandle Rotary::getUx()
		{
			if (!shown)
			{
				return control::ux::UxContainerHandle{};
			}
			std::map<std::string, std::string> replacements;
			replacements.emplace("@id", controller->getControlId(id));
			replacements.emplace("@name", name);
			replacements.emplace("@connection_name", controller->getConnectionName());
			replacements.emplace("@disabled", enabled ? "false" : "true");
			replacements.emplace("@rotary_id", controller->getControlId(id, "rotary"));
			replacements.emplace("@control_id", utils::getString(id));
			replacements.emplace("@left", utils::getString(left));
			replacements.emplace("@right", utils::getString(right));
			replacements.emplace("@value", utils::getString(value));
			replacements.emplace("@step", utils::getString(step));
			replacements.emplace("@fg_color",enabled?"#0d6efd":"#666666");
			return std::make_shared<control::ux::HtmlUxContainer>(controller->getEngine()->getTemplateWithReplacements("rotary", std::move(replacements)));

		}

		RotaryHandler::RotaryHandler(const control::ux::UIControllerHandle& controller_)
			: controller{ controller_ }
		{
			rotdayAdjustId = controller->getMapper()->registerType<RotaryAdjust>();
		}

		void RotaryHandler::handle(messages::MessageBase&& data)
		{
			endpoint::RotaryMessage & msg = static_cast<endpoint::RotaryMessage&>(data);

			if (msg.prop2.size() != 1)
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

			RotaryHandle rotary = std::make_shared<Rotary>(msg.prop2[0], msg.prop, rotdayAdjustId, controller);
			controller->addControl(rotary);
		}
	}
}
