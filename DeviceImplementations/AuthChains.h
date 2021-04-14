#ifndef AUTH_DEVICE_CHAINS_H
#define AUTH_DEVICE_CHAINS_H
#include <commproto/messages/TriplePropertyChain.h>
#include <commproto/messages/DoublePropertyChain.h>
#include <commproto/messages/EmptyChain.h>


MAKE_TRIPLE_PROP_MESSAGE(DeviceDataMessage, std::string, std::string, std::string);
using DeviceDataParser = commproto::messages::TriplePropertyParser<std::string, std::string, std::string>;
using DeviceDataSerializer = commproto::messages::TriplePropertySerializer<std::string, std::string, std::string>;

MAKE_DOUBLE_PROP_MESSAGE(ConnectionAuthorizedMessage, std::vector<std::string>, uint32_t);
using ConnectionAuthorizedParser = commproto::messages::DoublePropertyParser<std::vector<std::string>, uint32_t>;
using ConnectionAuthorizedSerializer = commproto::messages::DoublePropertySerializer<std::vector<std::string>, uint32_t>;

MAKE_EMPTY_MESSAGE(ConnectionRejectedMessage);
using ConnectionRejectedParser = commproto::messages::EmptyParser;
using ConnectionRejectedSerializer = commproto::messages::EmptySerializer;

namespace commproto
{
	DEFINE_DATA_TYPE(DeviceDataMessage);
	DEFINE_DATA_TYPE(ConnectionAuthorizedMessage);
	DEFINE_DATA_TYPE(ConnectionRejectedMessage);
}



#endif// AUTH_DEVICE_CHAINS_H