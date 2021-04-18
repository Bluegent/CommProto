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

				void send(Message && msg) const;
				void notifyUpdate() const;
			protected:
				UIController& manager;

			};

			inline void Generator::send(Message&& msg) const
			{
				manager.send(msg);
			}

			inline void Generator::notifyUpdate() const
			{
				manager.notifyUpdate();
			}

			template <typename ControlType>
			std::string Generator::generate(const ControlType& control) const
			{
				return "";
			}

			template <>
			inline std::string Generator::generate(const ButtonImpl& control) const
			{
				if (!control.isVisible())
				{
					return std::string();
				}

				std::stringstream sstream;
				sstream << "<button onclick = \"postButton('" << manager.getConnectionName() << "','" << control.getId() << "')\">" << control.getName() << " </button>";
				return sstream.str();
			}

			template <>
			inline std::string Generator::generate(const ToggleImpl& control) const
			{
				if (!control.isVisible())
				{
					return std::string();
				}

				std::stringstream sstream;
				sstream << manager.getConnectionName() << "-toggle" << control.getId();
				std::string controlIdString = sstream.str();
				sstream.clear();
				sstream.str(std::string());

				sstream << "<div class=\"toggle-switch\">";
				sstream << control.getName() << " : <input type=\"checkbox\" id=\"" << controlIdString << "\" onclick=\"postToggle(this, '" << manager.getConnectionName() << "', '" << control.getId() << "')\"" << (control.getState() ? " checked>" : " >");
				sstream << "<label for=\"" << controlIdString << "\"></label>";
				sstream << "</div>";
				return sstream.str();
			}

			template <>
			inline std::string Generator::generate(const LabelImpl& control) const
			{
				if (!control.isVisible())
				{
					return std::string();
				}

				std::stringstream sstream;
				sstream << "<span class=\"c_label\">" << control.getName() << ": " << control.getText() << "</span>";
				return sstream.str();
			}


			template <>
			inline std::string Generator::generate(const NotificationImpl& control) const
			{
				std::stringstream stream;

				stream << "notification-" << manager.getConnectionName() << "-" << control.getId();
				std::string notifId = stream.str();

				stream.clear();
				stream.str(std::string());

				stream << "onclick = \"invoke(event)\"";
				stream << "name='postNotification'";
				stream << "connectionId=\"" << manager.getConnectionName() << "\"";
				stream << "controlId=\"" << control.getId() << "\"";
				stream << "elementId=\"" << notifId << "\"";
				stream << "optionStr=\"";
				std::string attributes = stream.str();
				stream.clear();
				stream.str(std::string());

				stream << "<div class=\"notification\" id=\"" << notifId << "\">";
				stream << control.getName() << "<br>";
				auto options = control.getOptions();
				for (auto opt : options)
				{
					stream << "<button " << attributes << opt << "\" >" << opt << " </button>";
				}

				stream << "</div>";
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

				std::stringstream sstream;
				sstream << "slid-" << manager.getConnectionId() << "-" << control.getId();
				std::string sliderId = sstream.str();

				sstream.str(std::string());
				sstream.clear();
				
				sstream.precision(3);

				sstream << control.getName() << ":";
				sstream << "<div class=\"slidecontainer\">";
				sstream << left << " - " << right;
				sstream << "<input type=\"range\" class=\"slider\" ";
				sstream << "value=\"" << value << "\" ";
				sstream << "min=\"" << left << "\" ";
				sstream << "max=\"" << right << "\" ";
				sstream << "step=\"" << step << "\" ";
				sstream << "oninput=\"setSliderValue('" << sliderId<< "' ,this.value)\" ";
				sstream << "onmouseup=\"postSlider('" << manager.getConnectionName() << "', '" << control.getId() <<"', this.value, this.min, this.max)\"/>";
				sstream << "<div id=\""<< sliderId << "\"> value:" << value <<"</div>";
				sstream << "</div>";

				return sstream.str();
			}


			using GeneratorHandle = std::shared_ptr<Generator>;
		}
	}
}

#endif //UX_GENERATOR_H