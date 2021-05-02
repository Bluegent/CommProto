#include "PluginLoaderImpl.h"
#include <commproto/plugin/Definitions.h>
#include <DynamicModuleImpl.h>


namespace commproto
{
	namespace plugin
	{

		typedef void(CALL_CONV *SET_LOGGABLE)(logger::Loggable*);
		typedef void(CALL_CONV *EXTEND)(const commproto::control::ux::UXServiceProviderHandle & provider);

		PluginLoaderImpl::PluginLoaderImpl(): loggable{nullptr}
		{
		}

		void PluginLoaderImpl::setLoggable(logger::Loggable* loggable_)
		{
			loggable = loggable_;
		}

		void PluginLoaderImpl::load(const std::string& path, const control::ux::UXServiceProviderHandle& provider)
		{
			DynamicModuleHandle module = std::make_shared<DynamicModuleImpl>();
			bool open = module->open(path);
			if(!open)
			{
				LOG_ERROR("Could not open path %s", path.c_str());
			}
			setLoggableForModule(module);

			void* ptr = module->getFunction(PLUGIN_EXTEND_NAME);
			if (!ptr)
			{
				LOG_WARNING("Could find the extend function for library %s", module->getPath().c_str());
			}
			EXTEND extendFunc = static_cast<EXTEND>(ptr);
			if (!extendFunc)
			{
				LOG_WARNING("Extend function has wrong format", module->getPath().c_str());
			}
			extendFunc(provider);
			modules.push_back(module);
		}

		void PluginLoaderImpl::setLoggableForModule(const DynamicModuleHandle& module) const
		{
			if(!loggable)
			{
				return;
			}
			void* ptr = module->getFunction(PLUGIN_LOGGER_NAME);
			if (!ptr)
			{
				LOG_WARNING("Could find the set loggable function for library %s", module->getPath().c_str());
			}
			SET_LOGGABLE setL = static_cast<SET_LOGGABLE>(ptr);
			if(!setL)
			{
				LOG_WARNING("Set loggable function has wrong format", module->getPath().c_str());
			}
			setL(loggable);
		}
	}
}
