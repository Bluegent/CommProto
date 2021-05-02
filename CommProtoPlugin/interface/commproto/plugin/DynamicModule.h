#ifndef DYNAMIC_MODULE_H
#define DYNAMIC_MODULE_H
#include <string>
#include <memory>

namespace commproto
{
	namespace plugin
	{
		class DynamicModule
		{
		public:
			virtual ~DynamicModule() = default;
			virtual bool open(const std::string & path) = 0;
			virtual void close() = 0;
			virtual void* getFunction(const std::string & name) = 0;
			virtual std::string  getPath() const = 0;
		};
		using DynamicModuleHandle = std::shared_ptr<DynamicModule>;
	}
}



#endif//DYNAMIC_MODULE_H