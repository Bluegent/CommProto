#ifndef REQ_CHANNELS_HANDLER_H
#define REQ_CHANNELS_HANDLER_H

#include <commproto/parser/Handler.h>
#include <commproto/service/ChannelManager.h>

namespace commproto
{
	namespace service
	{
		class RequestAllChannelsHandler : public parser::Handler
		{
		public:
			RequestAllChannelsHandler(const ChannelManagerHandle & dispatch);
			void handle(messages::MessageBase&& data) override;
		private:
			ChannelManagerHandle dispatch;
		};
    }
}

#endif// REQ_CHANNELS_HANDLER_H