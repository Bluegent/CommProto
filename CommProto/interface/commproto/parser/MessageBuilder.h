#ifndef COMMPROTO_MESSAGE_BUILDER_H
#define COMMPROTO_MESSAGE_BUILDER_H

#include <commproto/common/Common.h>
#include <commproto/sockets/Socket.h>
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
			MessageBuilder(const sockets::SocketHandle& socket_, const ParserDelegatorHandle& delegator_);

			void pollAndRead();
			static uint8_t ptrSize;
		private:
			sockets::SocketHandle socket;
			State state;
			Message internal;
			uint32_t expectedReadSize;
			ParserDelegatorHandle delegator;

		};

		using MessageBuilderHandle = std::shared_ptr<MessageBuilder>;
	}
}


#endif