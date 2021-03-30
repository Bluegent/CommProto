#ifndef SEND_TO_HANDLER_H
#define SEND_TO_HANDLER_H

#include <commproto/parser/Handler.h>
#include <commproto/service/Dispatch.h>

namespace commproto
{
	namespace service
	{
		class SendToHandler : public parser::Handler
		{
		public:
			SendToHandler(Dispatch* dispatch);
			void handle(messages::MessageBase&& data) override;
		private:
			Dispatch* dispatch;
		};
    }
}

#endif// SEND_TO_HANDLER_H