#ifndef UX_CONTROLLERS_H
#define UX_CONTROLLERS_H
#include <string>
#include <map>
#include <commproto/control/UiController.h>

namespace commproto
{
	namespace control
	{
		namespace ux
		{
			class UxControllers
			{
			public:
				void addController(const std::string& name, const UIControllerHandle& controller);
				UIControllerHandle getController(const std::string& name);
				bool hasUpdate();

			private:
				std::map<std::string, commproto::control::ux::UIControllerHandle> controllers;
			};

			using UxControllersHandle = std::shared_ptr<UxControllers>;
		}
	}

}

#endif //UX_CONTROLLERS_H