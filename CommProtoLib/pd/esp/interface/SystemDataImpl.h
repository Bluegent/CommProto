#ifndef SYSTEM_DATA_IMPL_H
#define SYSTEM_DATA_IMPL_H
#include <commproto/diagnostics/SystemData.h>

namespace commproto
{
    namespace diagnostics
    {
        class SystemDataImpl : public SystemData
        {
        public:
			SystemDataImpl();
	        uint64_t getTotalMemBytes() override {return 0;}
	        uint64_t getUsedMemBytes() override {return 0;}
	        uint64_t getProcessMemBytes() override {return 0;}
	        float getProcessorUsage() override {return 0.f;}
	        float getProcessProcessorUsage() override {return 0.f;}
        };

    }
}

#endif // SYSTEM_DATA_IMPL_H