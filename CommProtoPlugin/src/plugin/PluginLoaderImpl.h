#ifndef PLUGIN_LOADER_IMPL_H
#define PLUGIN_LOADER_IMPL_H
#include <commproto/plugin/PluginLoader.h>
#include <commproto/plugin/DynamicModule.h>

namespace commproto
{
	namespace plugin
	{
		class PluginLoaderImpl : public PluginLoader
		{
		public:
			PluginLoaderImpl();

			void setLoggable(logger::Loggable * loggable) override;
			void load(const std::string & path, const control::ux::UXServiceProviderHandle& provider) override;
			void setStaticsForModules() override;
		private:

			void setStaticsForModule(const DynamicModuleHandle& module) const;
			
		private:
			logger::Loggable * loggable;
			std::vector<DynamicModuleHandle> modules;

		};
	}
}

#endif// PLUGIN_LOADER_H