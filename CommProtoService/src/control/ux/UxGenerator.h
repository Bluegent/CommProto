#ifndef UX_GENERATOR_H
#define UX_GENERATOR_H

#include <commproto/control/ux/UIController.h>
#include <sstream>

#include "ButtonImpl.h"
#include "ToggleImpl.h"
#include "LabelImpl.h"
#include "NotificationImpl.h"
#include "SliderImpl.h"
#include <commproto/utils/Math.h>

namespace commproto
{
	namespace control
	{
		namespace ux
		{
			class Generator
			{
			public:
				Generator(UIController& manager_)
					: manager(manager_)
				{

				}
				template <typename ControlType>
				std::string generate(const ControlType& control) const;

				template <typename ControlType>
				std::string getControlId(const ControlType& control) const;

				template <typename ControlType>
				std::string getControlTypeId(const ControlType& control) const;

				void send(Message && msg) const;
				void notifyUpdate(const uint32_t id) const;
			private:
				template <typename ControlType>
				std::map<std::string, std::string> getBaseReplacements(const ControlType& control) const;


				UIController& manager;

			};

			inline void Generator::send(Message&& msg) const
			{
				manager.send(msg);
			}

			inline void Generator::notifyUpdate(const uint32_t id) const
			{
				manager.notifyUpdate(id);
			}

			template <typename ControlType>
			inline std::map<std::string, std::string> Generator::getBaseReplacements(const ControlType& control)const
			{
				std::map<std::string, std::string>  replacements;
				replacements.emplace("@name", control.getName());
				replacements.emplace("@control_id", std::to_string(control.getId()));
				replacements.emplace("@id", getControlId(control));
				replacements.emplace("@connection_name", manager.getConnectionName());
				return replacements;
			}

			template <typename ControlType>
			std::string Generator::generate(const ControlType& control) const
			{
				return "";
			}

			template <typename ControlType>
			std::string Generator::getControlId(const ControlType& control) const
			{
				std::stringstream stream;

				stream << manager.getConnectionName() << "-" << control.getId();
				return stream.str();
			}

			template <typename ControlType>
			std::string Generator::getControlTypeId(const ControlType& control) const
			{
				return std::string();
			}

			template <>
			inline std::string Generator::getControlTypeId(const ToggleImpl& control) const
			{
				std::stringstream sstream;
				sstream << manager.getConnectionName() << "-toggle-" << control.getId();
				return sstream.str();
			}


			template <>
			inline std::string Generator::getControlTypeId(const SliderImpl& control) const
			{
				std::stringstream sstream;
				sstream << manager.getConnectionName() << "-slider-" << control.getId();
				return sstream.str();
			}


			template <>
			inline std::string Generator::generate(const ButtonImpl& control) const
			{
				if (!control.isVisible())
				{
					return std::string();
				}

				auto replacements = getBaseReplacements(control);

				return manager.getEngine()->getTemplateWithReplacements("button", std::move(replacements));
			}

			template <>
			inline std::string Generator::generate(const ToggleImpl& control) const
			{
				if (!control.isVisible())
				{
					return std::string();
				}

				auto replacements = getBaseReplacements(control);
				replacements.emplace("@toggle_id", getControlTypeId(control));
				replacements.emplace("@checked", control.getState() ? "checked" : "");

				return manager.getEngine()->getTemplateWithReplacements("toggle", std::move(replacements));

			}

			template <>
			inline std::string Generator::generate(const LabelImpl& control) const
			{
				if (!control.isVisible())
				{
					return std::string();
				}
				auto replacements = getBaseReplacements(control);
				replacements.emplace("@text", control.getText());
				return manager.getEngine()->getTemplateWithReplacements("label", std::move(replacements));

			}


			template <>
			inline std::string Generator::generate(const NotificationImpl& control) const
			{

				std::stringstream buttons;
				auto options = control.getOptions();
				for (auto opt : options)
				{
					auto replacements = getBaseReplacements(control);
					replacements.emplace("@option", opt);
					buttons << manager.getEngine()->getTemplateWithReplacements("notif_button", std::move(replacements));
				}

				auto replacements = getBaseReplacements(control);
				replacements.emplace("@buttons", buttons.str());
				replacements.emplace("@text", "pretend I implemented this :(");
				return manager.getEngine()->getTemplateWithReplacements("notification", std::move(replacements));
			}

			inline std::string getString(const float value, const uint32_t precision = 3)
			{
				std::stringstream stream;
				stream.precision(precision);
				stream << value;
				return stream.str();
			}

			template <>
			inline std::string Generator::generate(const SliderImpl& control) const
			{
				if (!control.isVisible())
				{
					return std::string();
				}

				float left, right, value, step;
				control.getValues(left, right, value, step);

				auto replacements = getBaseReplacements(control);
				replacements.emplace("@slider_id", getControlTypeId(control));
				replacements.emplace("@left", getString(left));
				replacements.emplace("@right", getString(right));
				replacements.emplace("@value", getString(value));
				replacements.emplace("@step", getString(step));

				return manager.getEngine()->getTemplateWithReplacements("slider", std::move(replacements));
			}


			using GeneratorHandle = std::shared_ptr<Generator>;
		}
	}
}

#endif //UX_GENERATOR_H