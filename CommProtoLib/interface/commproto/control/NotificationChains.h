#ifndef NOTIFICATION_CHAINS_H
#define NOTIFICATION_CHAINS_H

#include <commproto/messages/TriplePropertyChain.h>
#include <commproto/messages/DoublePropertyChain.h>

namespace commproto
{
    namespace control
    {
		namespace endpoint
		{
			//controlId, name, options
			MAKE_TRIPLE_PROP_MESSAGE(NotificationMessage, uint32_t, std::string, std::vector<std::string>);

			using NotificationSerializer = messages::TriplePropertySerializer<uint32_t, std::string, std::vector<std::string>>;
			using NotificationParser = messages::TriplePropertyParser<uint32_t, std::string, std::vector<std::string>>;

			//controlId, actionId, display text
			MAKE_TRIPLE_PROP_CHAIN(DisplayNotification, uint32_t, uint32_t, std::string);
		};

		namespace ux
		{
			MAKE_TRIPLE_PROP_CHAIN(NotificationResponse, uint32_t, uint32_t, std::string);
		}
    }

	DEFINE_DATA_TYPE(control::endpoint::NotificationMessage);
	DEFINE_DATA_TYPE(control::endpoint::DisplayNotification);
	DEFINE_DATA_TYPE(control::ux::NotificationResponse);
}


#endif// NOTIFICATION_CHAINS_H