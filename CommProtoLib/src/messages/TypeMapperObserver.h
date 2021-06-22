#ifndef COMMPROTO_TYPE_MAPPER_OBSERVER_H
#define COMMPROTO_TYPE_MAPPER_OBSERVER_H
#include <commproto/stream/Stream.h>
#include <commproto/parser/MappingType.h>
#include <commproto/logger/Logging.h>

namespace commproto
{
	namespace messages
	{

		class TypeMapperObserver
		{
		public:
			TypeMapperObserver(const stream::StreamHandle & stream_)
				: stream{ stream_ }
			{

			}

			void notify(const std::string & name, uint32_t id)
			{
				if (id == 0 || name == "")
				{
					return;
				}
				stream->sendBytes(MappingTypeSerializer::serialize(std::move(MappingType(name, id))));
			}
		private:
			stream::StreamHandle stream;
		};
		using TypeMapperObserverHandle = std::shared_ptr<TypeMapperObserver>;
	}
}
#endif // COMMPROTO_TYPE_MAPPER_OBSERVER_H