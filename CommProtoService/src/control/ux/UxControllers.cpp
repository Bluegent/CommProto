#include <commproto/control/ux/UxControllers.h>


namespace commproto
{
    namespace control
    {
        namespace ux
        {
	        void UxControllers::addController(const std::string& name, const UIControllerHandle& controller)
	        {
				std::lock_guard<std::mutex> lock(controllerMutex);
		        if (controllers.find(name) != controllers.end())
		        {
			        return;
		        }
				update = true;
		        controllers.emplace(name, controller);
	        }

	        void UxControllers::removeController(const std::string& name)
	        {
				std::lock_guard<std::mutex> lock(controllerMutex);
				auto it = controllers.find(name);
				if(it == controllers.end())
				{
					return;
				}
				it->second->clear();
				update = true;
				controllers.erase(it);
	        }

	        UIControllerHandle UxControllers::getController(const std::string& name)
	        {
				std::lock_guard<std::mutex> lock(controllerMutex);
		        auto it = controllers.find(name);

		        if (it == controllers.end())
		        {
			        return nullptr;
		        }
		        return it->second;
	        }

	        std::map<std::string, UIControllerHandle> UxControllers::getControllers()
	        {
				std::lock_guard<std::mutex> lock(controllerMutex);
				return controllers;
	        }
        }
    }
}