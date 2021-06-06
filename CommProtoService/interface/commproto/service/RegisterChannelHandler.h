#ifndef REGISTER_CHANNEL_HANDLER_H
#define REGISTER_CHANNEL_HANDLER_H

#include <commproto/parser/Handler.h>
#include <commproto/service/ChannelManager.h>

namespace commproto
{
	namespace service
	{
		class RegisterChannelHandler : public parser::Handler
		{
		public:
			RegisterChannelHandler(const ChannelManagerHandle & dispatch, const uint32_t id);
			void handle(messages::MessageBase&& data) override;
		private:
			ChannelManagerHandle dispatch;
			const uint32_t id;
		};
	}
}

#endif// REGISTER_CHANNEL_HANDLER_H