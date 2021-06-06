#ifndef SEND_TO_HANDLER_H
#define SEND_TO_HANDLER_H

#include <commproto/parser/Handler.h>
#include <commproto/service/ChannelManager.h>

namespace commproto
{
	namespace service
	{
		class SendToHandler : public parser::Handler
		{
		public:
			SendToHandler(const ChannelManagerHandle & dispatch);
			void handle(messages::MessageBase&& data) override;
		private:
			ChannelManagerHandle dispatch;
		};
    }
}

#endif// SEND_TO_HANDLER_H