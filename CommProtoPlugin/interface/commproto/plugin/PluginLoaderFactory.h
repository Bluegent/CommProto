#ifndef PLUGIN_LOADER_FACTORY_H
#define PLUGIN_LOADER_FACTORY_H
#include <commproto/plugin/PluginLoader.h>
#include <commproto/logger/Logging.h>

namespace commproto
{
	namespace plugin
	{
		class PluginLoaderFactory
		{
		public:
			static PluginLoaderHandle build(logger::Loggable * loggable = nullptr);
		};
	}
}


#endif // PLUGIN_LOADER_FACTORY_H