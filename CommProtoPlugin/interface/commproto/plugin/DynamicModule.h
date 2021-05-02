#ifndef DYNAMIC_MODULE_H
#define DYNAMIC_MODULE_H
#include <string>

namespace commproto
{
	namespace plugin
	{
		class DynamicModule
		{
		public:
			~DynamicModule() = default;
			virtual void open() = 0;
			virtual void close() = 0;
			virtual void* getFunction(const std::string & name) = 0;
		};
	}
}



#endif//DYNAMIC_MODULE_H