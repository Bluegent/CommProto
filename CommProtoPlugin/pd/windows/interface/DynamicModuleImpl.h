#ifndef DYNAMIC_MODULE_IMPL_H
#define DYNAMIC_MODULE_IMPL_H
#include <windows.h> 
#include <commproto/plugin/DynamicModule.h>


namespace commproto
{
	namespace plugin
	{
		class DynamicModuleImpl : public DynamicModule
		{
		public:
			DynamicModuleImpl();
			~DynamicModuleImpl();

			bool open(const std::string & path) override;
			void close() override;
			void* getFunction(const std::string& name) override;
			std::string getPath() const override;
		private:
			HINSTANCE dllInstance;
			std::string path;
		};
	}
}


#endif// DYNAMIC_MODULE_IMPL_H