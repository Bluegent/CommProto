#ifndef COMMPROTO_SERVICE_CHAINS_H
#define COMMPROTO_SERVICE_CHAINS_H

#include <commproto/messages/SinglePropertyChain.h>
#include <commproto/messages/DoublePropertyChain.h>


namespace commproto
{
	namespace service
	{

		//register channel
		MAKE_SINGLE_PROP_MESSAGE(RegisterChannelMessage,std::string);

		using RegisterChannelParser = messages::SinglePropertyParser<std::string>;

		using RegisterChannelSerializer = messages::SinglePropertySerializer<std::string>;

		//subscribe
		MAKE_SINGLE_PROP_MESSAGE(SubscribeMessage, std::string);

		using SubscribeParser = messages::SinglePropertyParser<std::string>;

		using SubscribeSerializer = messages::SinglePropertySerializer<std::string>;


		//unsubscribe
		MAKE_SINGLE_PROP_MESSAGE(UnsubscribeMessage, std::string);

		using UnsubscribeParser = messages::SinglePropertyParser<std::string>;

		using UnsubscribeSerializer = messages::SinglePropertySerializer<std::string>;

		//send to message
		MAKE_DOUBLE_PROP_MESSAGE(SendToMessage, uint32_t, Message);

		using SendtoParser = messages::DoublePropertyParser<uint32_t, Message>;

		using SendtoSerializer = messages::DoublePropertySerializer<uint32_t, Message>;

	}

	DEFINE_DATA_TYPE(service::RegisterChannelMessage);
	DEFINE_DATA_TYPE(service::SubscribeMessage);
	DEFINE_DATA_TYPE(service::UnsubscribeMessage);
	DEFINE_DATA_TYPE(service::SendToMessage);
	
}

#endif //COMMPROTO_SERVICE_CHAINS_H