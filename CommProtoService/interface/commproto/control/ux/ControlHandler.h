#ifndef CONTROL_HANDLER_H
#define CONTROL_HANDLER_H
#include <map>
#include <commproto/control/ux/Control.h>

namespace commproto
{
	namespace control
	{
		namespace ux
		{
			using AttributeMap = std::map<std::string, std::string>;
			

			struct ControlData
			{
				std::string tracker;
				std::string extension;
				std::string controlType;
				uint32_t id;
			};

			class ControlHandlerUtil
			{
			public:
				static ControlData getBase(const AttributeMap& attributes);
			};

			

			class UIController;
			class ControlHandler
			{
			public:
				virtual ~ControlHandler() = default;
				virtual void handle(AttributeMap&& attributes, UIController& controller) = 0;
			};

			using ControlHandlerHandle = std::shared_ptr<ControlHandler>;
		}
	}
}

#endif// CONTROL_HANDLER_H