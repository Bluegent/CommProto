#ifndef COMMPROTO_TYPE_MAPPER_H
#define COMMPROTO_TYPE_MAPPER_H
#include <string>
#include <stdint.h>
#include <memory>

#include <commproto/messages/MessageName.h>
#include <commproto/stream/Stream.h>

namespace commproto
{
	namespace messages
	{

		class TypeMapper
		{
		public:
			template <typename T>
			uint32_t registerType();

			template <typename T>
			bool hasType();
			virtual ~TypeMapper() = default;

			
		protected:
			virtual uint32_t registerType(const std::string& type) = 0;
			virtual bool hasType(const std::string& type) = 0;
		};

		using TypeMapperHandle = std::shared_ptr<TypeMapper>;

		class TypeMapperFactory
		{
		public:
			static TypeMapperHandle build(const stream::StreamHandle & stream);
		};

		template <typename T>
		uint32_t TypeMapper::registerType()
		{
			return registerType(MessageName<T>::name());
		}

		template <typename T>
		bool TypeMapper::hasType()
		{
			return hasType(MessageName<T>::name());
		}


		

	}
}

#endif // COMMPROTO_TYPE_MAPPER_H