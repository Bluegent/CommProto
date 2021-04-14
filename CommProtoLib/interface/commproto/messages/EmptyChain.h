#ifndef EMPTY_CHAIN_H
#define EMPTY_CHAIN_H

#include <commproto/parser/Parser.h>
#include <commproto/messages/EmptyMessage.h>
#include <commproto/common/Common.h>
#include <commproto/parser/ByteStream.h>

namespace commproto
{
	namespace messages
	{
		class EmptySerializer
		{
		public:
			static Message serialize(messages::MessageBase&& data);
		};

		inline Message EmptySerializer::serialize(messages::MessageBase&& data)
		{
			EmptyMessage& realData = static_cast<EmptyMessage&>(data);
			parser::ByteStream stream;
			stream.writeHeader(data);
			return stream.getStream();
		}


		class EmptyParser : public parser::Parser
		{
		public:
			EmptyParser(const parser::HandlerHandle& handler_)
				: Parser(handler_)
			{
			}

			void parse(parser::ByteStream&& msg) override;
		};

		inline void EmptyParser::parse(parser::ByteStream&& msg)
		{

			parser::ByteStream stream(msg);
			uint32_t sender = 0;
			stream.read < uint32_t>(sender);
			EmptyMessage realMessage(0);
			realMessage.senderId = sender;
			handler->handle(std::move(realMessage));
		}

	}
}

#endif// EMPTY_CHAIN_H