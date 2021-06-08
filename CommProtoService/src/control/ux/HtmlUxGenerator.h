#ifndef UX_GENERATOR_H
#define UX_GENERATOR_H

#include <commproto/control/ux/UIController.h>
#include <sstream>

#include "ButtonImpl.h"
#include "ToggleImpl.h"
#include "LabelImpl.h"
#include "NotificationImpl.h"
#include "SliderImpl.h"
#include "ProgressBarImpl.h"
#include  <commproto/control/ux/HtmlUxContainer.h>
#include "BaseControlType.h"
#include <commproto/utils/String.h>

namespace commproto
{
	namespace control
	{
		namespace ux
		{
			class HtmlGenerator : public UxGenerator
			{
			public:
				HtmlGenerator(UIController& manager_)
					: manager(manager_)
				{

				}

				UxContainerHandle generate(const Control& handle) override;
				UxContainerHandle generateNotification(const Control& handle, const std::string& text, const uint32_t actionId) override;

				template <typename ControlType>
				UxContainerHandle generateUx(const Control& handle);

				template <typename ControlType>
				std::string generate(const ControlType& control) const;
				std::string generateNotif(const NotificationImpl& control, const std::string & text, const uint32_t actionId);

				UxContainerHandle generateText(const std::string & text) const override;

				void send(Message && msg) const override;
				void notifyUpdate(const uint32_t id) const override;
			private:
				template <typename ControlType>
				std::map<std::string, std::string> getBaseReplacements(const ControlType& control) const;


			public:
				
			private:
				UIController& manager;

			};

			inline std::string encode(const std::string& data) {
				std::string buffer;
				buffer.reserve(data.size()*1.1);
				for (size_t pos = 0; pos != data.size(); ++pos) {
					switch (data[pos]) {
					case '&':  buffer.append("&amp;");       break;
					case '\"': buffer.append("&quot;");      break;
					case '\'': buffer.append("&apos;");      break;
					case '<':  buffer.append("&lt;");        break;
					case '>':  buffer.append("&gt;");        break;
					case '\370':  buffer.append("&deg;");        break;
					case '\n':  buffer.append("<br>");        break;
					default:   buffer.append(&data[pos], 1); break;
					}
				}
				return buffer;
			}


			inline UxContainerHandle HtmlGenerator::generate(const Control& handle)
			{
				BaseControlType type = static_cast<BaseControlType>(handle.getType());
				switch(type)
				{
				case BaseControlType::Button: 
					return generateUx<ButtonImpl>(handle);
				case BaseControlType::Label: 
					return generateUx<LabelImpl>(handle);
				case BaseControlType::ProgressBar: 
					return generateUx<ProgressBarImpl>(handle);
				case BaseControlType::Slider: 
					return generateUx<SliderImpl>(handle);
				case BaseControlType::Toggle: 
					return generateUx<ToggleImpl>(handle);
				default: ;
				}
				return nullptr;
			}


			template <typename ControlType>
			UxContainerHandle HtmlGenerator::generateUx(const Control& handle)
			{
				return std::make_shared<HtmlUxContainer>(generate<ControlType>(static_cast<const ControlType &>(handle)));
			}


			inline UxContainerHandle HtmlGenerator::generateNotification(const Control& handle, const std::string& text, const uint32_t actionId)
			{
				return std::make_shared<HtmlUxContainer>(generateNotif(static_cast<const NotificationImpl&>(handle), text, actionId));
			}

			inline std::string HtmlGenerator::generateNotif(const NotificationImpl& control, const std::string& text, const uint32_t actionId)
			{
				std::stringstream buttons;
				auto options = control.getOptions();
				std::string elementId = manager.getControlId(actionId, "notif");
				for (auto opt : options)
				{
					auto replacements = getBaseReplacements(control);
					replacements.emplace("@option", opt);
					replacements.emplace("@actionId", std::to_string(actionId));
					replacements.emplace("@@elem_id", manager.getControlId(actionId, "notif"));
					buttons << manager.getEngine()->getTemplateWithReplacements("notif_button", std::move(replacements));
				}

				auto replacements = getBaseReplacements(control);
				replacements.emplace("@buttons", buttons.str());
				replacements.emplace("@text", encode(text));
				replacements.emplace("@elem_id", manager.getControlId(actionId, "notif"));
				return manager.getEngine()->getTemplateWithReplacements("notification", std::move(replacements));
			}

			inline UxContainerHandle HtmlGenerator::generateText(const std::string& text) const
			{
				std::map<std::string, std::string>  replacements;
				replacements.emplace("@text", encode(text));
				replacements.emplace("@id", manager.getControlId(0));
				return std::make_shared<HtmlUxContainer>(manager.getEngine()->getTemplateWithReplacements("text-line", std::move(replacements)));
			}

			inline void HtmlGenerator::send(Message&& msg) const
			{
				manager.send(msg);
			}

			inline void HtmlGenerator::notifyUpdate(const uint32_t id) const
			{
				manager.notifyUpdate(id);
			}

			template <typename ControlType>
			inline std::map<std::string, std::string> HtmlGenerator::getBaseReplacements(const ControlType& control)const
			{
				std::map<std::string, std::string>  replacements;
				replacements.emplace("@name", encode(control.getName()));
				replacements.emplace("@control_id", std::to_string(control.getId()));
				replacements.emplace("@id", manager.getControlId(control.getId()));
				replacements.emplace("@connection_name", manager.getConnectionName());

				std::string disabledStr = control.isEnabled() ? "" : "disabled";
				replacements.emplace("@disabled", disabledStr);
				return replacements;
			}


			template <typename ControlType>
			std::string HtmlGenerator::generate(const ControlType& control) const
			{
				return "";
			}

			template <>
			inline std::string HtmlGenerator::generate(const ButtonImpl& control) const
			{
				if (!control.isVisible())
				{
					return std::string();
				}

				auto replacements = getBaseReplacements(control);

				return manager.getEngine()->getTemplateWithReplacements("button", std::move(replacements));
			}

			template <>
			inline std::string HtmlGenerator::generate(const ToggleImpl& control) const
			{
				if (!control.isVisible())
				{
					return std::string();
				}

				auto replacements = getBaseReplacements(control);
				replacements.emplace("@toggle_id", manager.getControlId(control.getId(), "toggle"));
				replacements.emplace("@checked", control.getState() ? "checked" : "");

				return manager.getEngine()->getTemplateWithReplacements("toggle", std::move(replacements));

			}

			template <>
			inline std::string HtmlGenerator::generate(const LabelImpl& control) const
			{
				if (!control.isVisible())
				{
					return std::string();
				}
				auto replacements = getBaseReplacements(control);
				std::string text = control.getText();
				replacements.emplace("@text", encode(text));
				return manager.getEngine()->getTemplateWithReplacements("label", std::move(replacements));

			}

			template <>
			inline std::string HtmlGenerator::generate(const SliderImpl& control) const
			{
				if (!control.isVisible())
				{
					return std::string();
				}

				float left, right, value, step;
				control.getValues(left, right, value, step);

				auto replacements = getBaseReplacements(control);
				replacements.emplace("@slider_id", manager.getControlId(control.getId(), "slider"));
				replacements.emplace("@left", utils::getString(left));
				replacements.emplace("@right", utils::getString(right));
				replacements.emplace("@mid", utils::getString((right + left) / 2.f));
				replacements.emplace("@value", utils::getString(value));
				replacements.emplace("@step", utils::getString(step));
				replacements.emplace("@unit", encode(control.getUnitOfMeasure()));

				return manager.getEngine()->getTemplateWithReplacements("slider", std::move(replacements));
			}

			template <>
			inline std::string HtmlGenerator::generate(const ProgressBarImpl& control) const
			{
				if (!control.isVisible())
				{
					return std::string();
				}

				auto replacements = getBaseReplacements(control);
				replacements.emplace("@value", utils::getString(control.getProgress()));

				return manager.getEngine()->getTemplateWithReplacements("progress_bar", std::move(replacements));
			}
		}
	}
}

#endif //UX_GENERATOR_H