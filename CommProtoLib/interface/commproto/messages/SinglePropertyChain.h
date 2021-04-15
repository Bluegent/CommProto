#ifndef SINGLE_PROPERTY_CHAIN_H
#define SINGLE_PROPERTY_CHAIN_H

#include <commproto/parser/Parser.h>
#include <commproto/messages/SinglePropertyMessage.h>
#include <commproto/common/Common.h>
#include <commproto/parser/ByteStream.h>

namespace commproto
{
	namespace messages
	{
		template <typename  T>
		class SinglePropertySerializer
		{
		public:
			static Message serialize(messages::MessageBase&& data);
		};

		template <typename T>
		Message SinglePropertySerializer<T>::serialize(messages::MessageBase&& data)
		{
			SinglePropertyMessage<T>& realData = static_cast<SinglePropertyMessage<T>&>(data);
			parser::ByteStream stream;
			stream.writeHeader(data);
			stream.write(realData.prop);
			return stream.getStream();
		}


		template <typename  T>
		class SinglePropertyParser : public parser::Parser
		{
		public:
			SinglePropertyParser(const parser::HandlerHandle& handler_)
				: Parser(handler_)
			{
			}

			void parse(parser::ByteStream&& msg) override;
		};

		template <typename T>
		void SinglePropertyParser<T>::parse(parser::ByteStream&& msg)
		{

			parser::ByteStream stream(msg);
			uint32_t sender = 0;
			T prop;
			stream.read < uint32_t>(sender);
			stream.read(prop);
			SinglePropertyMessage<T> realMessage(0, prop);
			realMessage.senderId = sender;
			handler->handle(std::move(realMessage));
		}

	}
}

#define MAKE_SINGLE_PROP_CHAIN(Name, Type1)												\
	MAKE_SINGLE_PROP_MESSAGE(Name,Type1);												\
	using Name ## Parser = commproto::messages::SinglePropertyParser<Type1>;			\
	using Name ## Serializer = commproto::messages::SinglePropertySerializer<Type1>;	\

#endif// SINGLE_PROPERTY_CHAIN_H