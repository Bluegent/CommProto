#include <commproto/service/SendToHandler.h>
#include <commproto/service/ServiceChains.h>

namespace commproto
{
namespace service
{
	SendToHandler::SendToHandler(const ChannelManagerHandle & dispatch_) : dispatch{ dispatch_ }
	{
	}

	void SendToHandler::handle(messages::MessageBase&& data)
	{
		SendToMessage& message = static_cast<SendToMessage&>(data);
		if (dispatch) {
			dispatch->sendTo(message.senderId,message.prop, message.prop2);
		}
	}
}
}
