#ifndef REQ_CHANNELS_HANDLER_H
#define REQ_CHANNELS_HANDLER_H

#include <commproto/parser/Handler.h>
#include <commproto/service/Dispatch.h>

namespace commproto
{
	namespace service
	{
		class RequestAllChannelsHandler : public parser::Handler
		{
		public:
			RequestAllChannelsHandler(Dispatch& dispatch);
			void handle(messages::MessageBase&& data) override;
		private:
			Dispatch& dispatch;
		};
    }
}

#endif// REQ_CHANNELS_HANDLER_H