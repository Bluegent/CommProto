#ifndef SYSTEM_DATA_H
#define SYSTEM_DATA_H
#include <memory>

namespace commproto
{
    namespace diagnostics
    {
		class SystemData;
		using SystemDataHandle = std::shared_ptr<SystemData>;

        class SystemData
        {
        public:
            virtual ~SystemData() = default;
            virtual uint64_t getTotalMemBytes() = 0;
            virtual uint64_t getUsedMemBytes() = 0;
            virtual uint64_t getProcessMemBytes() = 0;
            virtual float getProcessorUsage() = 0;
            virtual float getProcessProcessorUsage() = 0;
			static SystemDataHandle build();
		};
        
        
    }
}

#endif // SYSTEM_DATA_H