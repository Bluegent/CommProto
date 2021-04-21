#ifndef UX_CONTROLLERS_H
#define UX_CONTROLLERS_H
#include <map>
#include <commproto/control/ux/UIController.h>
#include <mutex>

namespace commproto
{
	namespace control
	{
		namespace ux
		{
			class UxControllers
			{
			public:
				UxControllers()
					: update(true)
				{
				}

				void addController(const std::string& name, const UIControllerHandle& controller);
				void removeController(const std::string & name);
				UIControllerHandle getController(const std::string& name);
				std::map<std::string, UIControllerHandle> getControllers();

			private:
				std::map<std::string, UIControllerHandle> controllers;
				bool update;
				std::mutex controllerMutex;
				
			};

			using UxControllersHandle = std::shared_ptr<UxControllers>;
		}
	}

}

#endif //UX_CONTROLLERS_H
