#ifndef TRIPLE_PROPERTY_CHAIN_H
#define TRIPLE_PROPERTY_CHAIN_H

#include <commproto/parser/Parser.h>
#include <commproto/messages/TriplePropertyMessage.h>
#include <commproto/common/Common.h>
#include <commproto/parser/ByteStream.h>

namespace commproto
{
	namespace messages
	{

		template <typename  T, typename  U, typename V>
		class TriplePropertySerializer
		{
		public:
			static Message serialize(messages::MessageBase&& data);
		};

		template <typename  T, typename  U, typename V>
		Message TriplePropertySerializer<T, U, V>::serialize(messages::MessageBase&& data)
		{
			TriplePropertyMessage<T, U, V>& realData = static_cast<TriplePropertyMessage<T, U, V>&>(data);
			parser::ByteStream stream;
			stream.writeHeader(data);
			stream.write(realData.prop);
			stream.write(realData.prop2);
			stream.write(realData.prop3);
			return stream.getStream();
		}

		template <typename  T, typename U, typename V>
		class TriplePropertyParser : public parser::Parser
		{
		public:
			TriplePropertyParser(const parser::HandlerHandle& handler_)
				: Parser(handler_)
			{
			}

			void parse(parser::ByteStream&& msg) override;
		};

		template <typename  T, typename U, typename V>
		void TriplePropertyParser<T, U, V>::parse(parser::ByteStream&& msg)
		{

			parser::ByteStream stream(msg);
			uint32_t sender = 0;
			T prop;
			U prop2;
			V prop3;
			stream.read < uint32_t>(sender);
			stream.read(prop);
			stream.read(prop2);
			stream.read(prop3);
			TriplePropertyMessage<T, U, V> realMessage(0, prop, prop2, prop3);
			realMessage.senderId = sender;
			handler->handle(std::move(realMessage));
		}
	}
}

#define MAKE_TRIPLE_PROP_CHAIN(Name, Type1,Type2,Type3)												\
	MAKE_TRIPLE_PROP_MESSAGE(Name,Type1,Type2,Type3);												\
	using Name ## Parser = commproto::messages::TriplePropertyParser<Type1,Type2,Type3>;			\
	using Name ## Serializer = commproto::messages::TriplePropertySerializer<Type1,Type2,Type3>;	\

#endif// TRIPLE_PROPERTY_CHAIN_H