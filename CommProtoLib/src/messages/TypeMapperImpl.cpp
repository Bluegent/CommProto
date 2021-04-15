#include "TypeMapperImpl.h"

namespace commproto
{
	namespace messages
	{
		TypeMapperImpl::TypeMapperImpl(const TypeMapperObserverHandle& observer_)
			: counter{ 0 }
			, observer{ observer_ }
		{
			TypeMapper::registerType<MappingType>();
		}

		uint32_t TypeMapperImpl::registerType(const std::string& type)
		{
			auto inMap = types.find(type);

			if (inMap != types.end()) {
				return inMap->second;
			}

			uint32_t id = counter++;
			types.emplace(type, id);
			if (id != 0) {
				observer->notify(type, id);
			}
			return id;
		}

		bool TypeMapperImpl::hasType(const std::string& type)
		{
			return types.find(type) != types.end();
		}
	}
}