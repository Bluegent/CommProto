#ifndef AUTH_DEVICE_CHAINS_H
#define AUTH_DEVICE_CHAINS_H
#include <commproto/messages/TriplePropertyChain.h>
#include <commproto/messages/DoublePropertyChain.h>
#include <commproto/messages/EmptyChain.h>
#include <commproto/messages/SinglePropertyChain.h>

namespace commproto
{
	namespace device {
		MAKE_TRIPLE_PROP_MESSAGE(DeviceDataMessage, std::string, std::string, std::string);
		using DeviceDataParser = messages::TriplePropertyParser<std::string, std::string, std::string>;
		using DeviceDataSerializer = messages::TriplePropertySerializer<std::string, std::string, std::string>;

		MAKE_SINGLE_PROP_MESSAGE(DeviceAuthRequestMessage, std::vector<std::string>);
		using DeviceAuthRequestParser = messages::SinglePropertyParser<std::vector<std::string>>;
		using DeviceAuthRequestSerializer = messages::SinglePropertySerializer<std::vector<std::string>>;

		MAKE_DOUBLE_PROP_MESSAGE(ConnectionAuthorizedMessage, std::vector<std::string>, uint32_t);
		using ConnectionAuthorizedParser = messages::DoublePropertyParser<std::vector<std::string>, uint32_t>;
		using ConnectionAuthorizedSerializer = messages::DoublePropertySerializer<std::vector<std::string>, uint32_t>;

		MAKE_EMPTY_MESSAGE(ConnectionRejectedMessage);
		using ConnectionRejectedParser = messages::EmptyParser;
		using ConnectionRejectedSerializer = messages::EmptySerializer;

		MAKE_EMPTY_MESSAGE(ScanForNetworksMessage);
		using ScanForNetworksParser = messages::EmptyParser;
		using ScanForNetworksSerializer = messages::EmptySerializer;


		MAKE_TRIPLE_PROP_CHAIN(DeviceAuthAccept, std::string, std::vector<std::string>, uint32_t);
		MAKE_SINGLE_PROP_CHAIN(DeviceAuthReject, std::string);
		MAKE_EMPTY_CHAIN(ScanFinished);

	}

	DEFINE_DATA_TYPE(device::DeviceDataMessage);
	DEFINE_DATA_TYPE(device::ConnectionAuthorizedMessage);
	DEFINE_DATA_TYPE(device::ConnectionRejectedMessage);
	DEFINE_DATA_TYPE(device::ScanForNetworksMessage);
	DEFINE_DATA_TYPE(device::DeviceAuthRequestMessage);
	DEFINE_DATA_TYPE(device::DeviceAuthAccept);
	DEFINE_DATA_TYPE(device::DeviceAuthReject);
	DEFINE_DATA_TYPE(device::ScanFinished);
}



#endif// AUTH_DEVICE_CHAINS_H