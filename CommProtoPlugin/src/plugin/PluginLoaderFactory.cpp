#include <commproto/plugin/PluginLoaderFactory.h>
#include "PluginLoaderImpl.h"


namespace commproto
{
	namespace plugin
	{
		PluginLoaderHandle PluginLoaderFactory::build(logger::Loggable* loggable)
		{
			auto loader = std::make_shared<PluginLoaderImpl>();
			loader->setLoggable(loggable);
			return loader;
		}
	}
}

