#ifndef DOUBLE_PROPERTY_CHAIN_H
#define DOUBLE_PROPERTY_CHAIN_H

#include <commproto/parser/Parser.h>
#include <commproto/messages/DoublePropertyMessage.h>
#include <commproto/common/Common.h>
#include <commproto/parser/ByteStream.h>

namespace commproto
{
	namespace messages
	{

		template <typename  T, typename  U>
		class DoublePropertySerializer
		{
		public:
			static Message serialize(MessageBase&& data);
		};

		template <typename T, typename  U>
		Message DoublePropertySerializer<T, U>::serialize(messages::MessageBase&& data)
		{
			DoublePropertyMessage<T, U>& realData = static_cast<DoublePropertyMessage<T, U>&>(data);
			parser::ByteStream stream;
			stream.writeHeader(data);
			stream.write(realData.prop);
			stream.write(realData.prop2);
			return stream.getStream();
		}

		template <typename  T, typename U>
		class DoublePropertyParser : public parser::Parser
		{
		public:
			DoublePropertyParser(const parser::HandlerHandle& handler_)
				: Parser(handler_)
			{
			}

			void parse(parser::ByteStream&& msg) override;
		};

		template <typename  T, typename U>
		void DoublePropertyParser<T, U>::parse(parser::ByteStream&& msg)
		{

			parser::ByteStream stream(msg);
			uint32_t sender = 0;
			T prop;
			U prop2;
			stream.read < uint32_t>(sender);
			stream.read(prop);
			stream.read(prop2);
			DoublePropertyMessage<T, U> realMessage(0, prop, prop2);
			realMessage.senderId = sender;
			handler->handle(std::move(realMessage));
		}

	}
}


#define MAKE_DOUBLE_PROP_CHAIN(Name, Type1,Type2)											\
	MAKE_DOUBLE_PROP_MESSAGE(Name,Type1,Type2);												\
	using Name ## Parser = commproto::messages::DoublePropertyParser<Type1,Type2>;			\
	using Name ## Serializer = commproto::messages::DoublePropertySerializer<Type1,Type2>;	\

#endif// DOUBLE_PROPERTY_CHAIN_H