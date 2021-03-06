#ifndef COMMMPROTO_MAPPING_TYPE_H
#define COMMMPROTO_MAPPING_TYPE_H

#include <commproto/messages/MessageName.h>
#include <commproto/messages/MessageBase.h>
#include <commproto/common/Common.h>
#include <commproto/parser/ByteStream.h>

namespace  commproto
{
	namespace messages
	{
		class MappingType : public MessageBase
		{
		public:
			std::string name;
			uint32_t typeId;

			MappingType(const std::string& name_, const uint32_t id_)
				: MessageBase{ 0 } //0 is reserved for mapping types
				, name{ name_ }
				, typeId{ id_ } {}


			BP_TYPE_DEFAULTS(MappingType);

			uint32_t getSize() const override {
				return sizeOf(name)
					+ sizeof(typeId)
					+ MessageBase::getSize();
			}
		};

		class MappingTypeSerializer
		{
		public:
			static Message serialize(MessageBase && data)
			{
				MappingType& mapping = static_cast<MappingType&>(data);
				parser::ByteStream stream;
				stream.writeHeader(data);
				stream.write(mapping.name);
				stream.write(mapping.typeId);
				return stream.getStream();
			}
		};

	}
	DEFINE_DATA_TYPE(messages::MappingType);
};

#endif //COMMMPROTO_MAPPING_TYPE_H