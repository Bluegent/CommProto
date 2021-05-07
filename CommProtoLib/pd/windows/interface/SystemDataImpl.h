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
	        uint64_t getTotalMemBytes() override;
	        uint64_t getUsedMemBytes() override;
	        uint64_t getProcessMemBytes() override;
	        float getProcessorUsage() override;
	        float getProcessProcessorUsage() override;
        };

    }
}

#endif // SYSTEM_DATA_IMPL_H