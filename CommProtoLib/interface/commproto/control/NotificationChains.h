#ifndef NOTIFICATION_CHAINS_H
#define NOTIFICATION_CHAINS_H

#include <commproto/messages/TriplePropertyChain.h>
#include <commproto/messages/DoublePropertyChain.h>
#include <commproto/messages/SinglePropertyChain.h>

namespace commproto
{
    namespace control
    {
		namespace endpoint
		{
			MAKE_TRIPLE_PROP_MESSAGE(NotificationMessage, uint32_t, std::string, std::vector<std::string>);

			using NotificationSerializer = messages::TriplePropertySerializer<uint32_t, std::string, std::vector<std::string>>;
			using NotificationParser = messages::TriplePropertyParser<uint32_t, std::string, std::vector<std::string>>;

			MAKE_SINGLE_PROP_MESSAGE(DisplayNotificationMessage, uint32_t);
			using DisplayNotificationSerializer = messages::SinglePropertySerializer<uint32_t>;
			using DisplayNotificationParser = messages::SinglePropertyParser<uint32_t>;
		}

		namespace ux
		{
			MAKE_DOUBLE_PROP_MESSAGE(NotificationResponseMessage, uint32_t, std::string);
			using NotificationResponseSerializer = messages::DoublePropertySerializer<uint32_t, std::string>;
			using NotificationResponseParser = messages::DoublePropertyParser<uint32_t, std::string>;
		}
    }

	DEFINE_DATA_TYPE(control::endpoint::NotificationMessage);
	DEFINE_DATA_TYPE(control::endpoint::DisplayNotificationMessage);
	DEFINE_DATA_TYPE(control::ux::NotificationResponseMessage);
}


#endif// NOTIFICATION_CHAINS_H