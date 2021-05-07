#include "RequestAllChannelsHandler.h"
#include <commproto/service/DiagnosticChains.h>

namespace commproto
{
	namespace service
	{
		RequestAllChannelsHandler::RequestAllChannelsHandler(Dispatch& dispatch_)
			: dispatch(dispatch_)
		{
		}

		void RequestAllChannelsHandler::handle(messages::MessageBase&& data)
		{
			auto connection = dispatch.getConnection(data.senderId);
			if(!connection)
			{
				return;
			}
			connection->sendAllChannels(dispatch.getMapping());
			
		}
	}
}
