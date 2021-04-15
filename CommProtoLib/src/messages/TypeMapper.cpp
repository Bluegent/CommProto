#include <commproto/messages/TypeMapper.h>
#include "TypeMapperImpl.h"

namespace commproto
{
	namespace messages
	{

		TypeMapperHandle TypeMapperFactory::build(const stream::StreamHandle & stream)
		{
			TypeMapperObserverHandle observer = std::make_shared<TypeMapperObserver>(stream);
			return std::make_shared<messages::TypeMapperImpl>(observer);
		}
	}
}