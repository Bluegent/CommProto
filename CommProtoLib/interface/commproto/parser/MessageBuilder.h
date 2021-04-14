#ifndef COMMPROTO_MESSAGE_BUILDER_H
#define COMMPROTO_MESSAGE_BUILDER_H

#include <commproto/common/Common.h>
#include <commproto/stream/Stream.h>
#include <commproto/parser/ParserDelegator.h>

namespace commproto
{
	namespace parser
	{
		enum class State : uint8_t
		{
			ReadingHandshake,
			ReadingPacketSize,
			ReadingPayload,
		};

		class MessageBuilder
		{
		public:
			MessageBuilder(const stream::StreamHandle& stream_, const ParserDelegatorHandle& delegator_);

			bool pollAndRead();
			bool pollAndReadTimes(const uint32_t times);
			static uint8_t ptrSize;
		private:
			stream::StreamHandle stream;
			State state;
			Message buffer;
			Message internal;
			uint32_t expectedReadSize;
			ParserDelegatorHandle delegator;

		};

		using MessageBuilderHandle = std::shared_ptr<MessageBuilder>;
	}
}


#endif