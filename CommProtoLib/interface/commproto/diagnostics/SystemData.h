#ifndef SYSTEM_DATA_H
#define SYSTEM_DATA_H
#include <memory>

namespace commproto
{
    namespace diagnostics
    {
        class SystemData
        {
        public:
            virtual ~SystemData() = default;
            virtual uint32_t getTotalMemBytes() = 0;
            virtual uint32_t getUsedMemBytes() = 0;
            virtual uint32_t getProcessMemBytes() = 0;
            virtual float getProcessorUsage() = 0;
            virtual float getProcessProcessorUsage() = 0;
        }
        
        using SystemDataHandle = std::shared_ptr<SystemData>;
    }
}

#endif // SYSTEM_DATA_H