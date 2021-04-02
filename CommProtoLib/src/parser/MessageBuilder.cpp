#include <commproto/parser/MessageBuilder.h>
#include <commproto/logger/Logging.h>

namespace commproto
{
	namespace parser
	{

		uint8_t MessageBuilder::ptrSize = sizeof(void*);

		MessageBuilder::MessageBuilder(const sockets::SocketHandle& socket_, const ParserDelegatorHandle& delegator_)
			: socket{ socket_ }
			, state{ State::ReadingHandshake }
			, expectedReadSize{ 0 }
			, delegator{ delegator_ }
		{
		}

		bool MessageBuilder::pollAndRead()
		{
            while(socket->pollSocket() > 0){
				char byte = socket->readByte();
				switch (state)
				{
				case State::ReadingHandshake:
				{
					ptrSize = byte;
					state = State::ReadingPacketSize;
					expectedReadSize = sizeof(uint32_t);
				}
				break;
				case State::ReadingPacketSize:
				{
					if (internal.size() < expectedReadSize)
					{
						internal.push_back(byte);
					}
					if (internal.size() == expectedReadSize)
					{
						ByteStream stream(internal);
						stream.read(expectedReadSize);
						if (expectedReadSize <= 0)
						{
							LOG_ERROR("Possible error: 0 bytes read expected.");
							internal.clear();
							return false;
						}
						internal.clear();
						state = State::ReadingPayload;
					}
				}
				break;
				case State::ReadingPayload:
				{
					if (internal.size() < expectedReadSize)
					{
						internal.push_back(byte);
					}
					if (internal.size() == expectedReadSize)
					{
						delegator->parse(internal);
						internal.clear();
						expectedReadSize = sizeof(uint32_t);
						state = State::ReadingPacketSize;
					}
				}
				break;
				}
            }
			return true;
		}

		bool MessageBuilder::pollAndReadTimes(const uint32_t times)
		{
			for(uint32_t index = 0; index < times ;++index)
			{
				if(!pollAndRead())
				{
					return false;
				}
			}
			return true;
		}
	}
}
