#ifndef PLUGIN_LOADER_H
#define PLUGIN_LOADER_H
#include <commproto/control/ux/UxDelegatorProvider.h>
#include <commproto/logger/Logging.h>

namespace commproto
{
	namespace plugin
	{
		class PluginLoader 
		{
		public:
			virtual ~PluginLoader() = default;
			virtual void setLoggable(logger::Loggable * loggable) = 0;
			virtual void load(const std::string & path, const control::ux::UXServiceProviderHandle& provider) = 0;
		};
		using PluginLoaderHandle = std::shared_ptr<PluginLoader>;
	}
}

#endif// PLUGIN_LOADER_H