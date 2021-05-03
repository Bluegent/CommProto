#include "PluginLoaderImpl.h"
#include <commproto/plugin/Definitions.h>
#include <DynamicModuleImpl.h>


namespace commproto
{
	namespace plugin
	{

		typedef void(CALL_CONV *SET_STATICS)(logger::Loggable*, const char * name, const uint32_t id);
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

		void PluginLoaderImpl::setStaticsForModules()
		{
			for(auto module : modules)
			{
				setStaticsForModule(module);
			}
		}

		void PluginLoaderImpl::setStaticsForModule(const DynamicModuleHandle& module) const
		{
			void* ptr = module->getFunction(PLUGIN_STATICS_NAME);
			if (!ptr)
			{
				LOG_WARNING("Could find the set loggable function for library %s", module->getPath().c_str());
				return;
			}
			SET_STATICS setL = static_cast<SET_STATICS>(ptr);
			if(!setL)
			{
				LOG_WARNING("Set loggable function has wrong format", module->getPath().c_str());
				return;
			}
			setL(loggable,SenderMapping::getName().c_str(),SenderMapping::getId());
		}
	}
}
