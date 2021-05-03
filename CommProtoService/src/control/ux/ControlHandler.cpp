#include <commproto/control/ux/ControlHandler.h>
namespace commproto
{
	namespace control
	{
		namespace ux
		{
			ControlData ControlHandlerUtil::getBase(const AttributeMap& attributes)
			{
				ControlData result{};

				auto type = attributes.find("controlType");
				if (type != attributes.end())
				{
					result.controlType = type->second;
				}
				auto ext = attributes.find("extension");
				if (ext != attributes.end())
				{
					result.extension = ext->second;
				}

				auto tracker = attributes.find("session");
				if (tracker != attributes.end())
				{
					result.tracker = tracker->second;
				}
				auto id = attributes.find("controlId");
				if (id != attributes.end())
				{
					try
					{
						result.id = std::stoi(id->second);
					}
					catch (std::invalid_argument arg)
					{
						result.id = 0;
					}
				}
				return result;

			}
		}
	}
}