#include "AuthServiceImpl.h"
#include <commproto/device/AuthChains.h>
#include <commproto/logger/Logging.h>

AuthServiceImpl::AuthServiceImpl(const commproto::stream::StreamHandle stream_)
	: stream{ stream_ }
	, mapper{ commproto::messages::TypeMapperFactory::build(stream) }
	, scanning{ false }
	, scanId{ mapper->registerType<commproto::device::ScanForNetworksMessage>() }
	, authorizeId{ mapper->registerType<commproto::device::DeviceAuthAccept>() }
	, rejectId{ mapper->registerType<commproto::device::DeviceAuthReject>() }
	, keepAliveId{ mapper->registerType<commproto::device::KeepAlive>() }
{
}

void AuthServiceImpl::scan()
{
	if (scanning)
	{
		return;
	}
	scanning = true;
	commproto::Message scan = commproto::device::ScanForNetworksSerializer::serialize(std::move(commproto::device::ScanForNetworksMessage(scanId)));
	stream->sendBytes(scan);
}

void AuthServiceImpl::setScanFinished()
{
	scanning = false;
}

void AuthServiceImpl::handleRequest(const APData& data)
{
	LOG_INFO("Name: \"%s\" (ssid:%s)", data.name.c_str(), data.ssid.c_str());
	LOG_INFO("Manufacturer: \"%s\"", data.manufacturer.c_str());
	LOG_INFO("Description: \"%s\"", data.description.c_str());
	accept(data.ssid);
}

void AuthServiceImpl::accept(const std::string& name)
{
	std::vector<std::string> props;
	props.push_back("EstiNebun"); //ssid of hub
	props.push_back("01LMS222"); //password for hub
	props.push_back("192.168.1.2"); //dispatch address
	uint32_t port = 25565; //dispatch port
	commproto::Message accept = commproto::device::DeviceAuthAcceptSerializer::serialize(std::move(commproto::device::DeviceAuthAccept(authorizeId, name, props, port)));
	stream->sendBytes(accept);
}

void AuthServiceImpl::reject(const std::string& name)
{
	commproto::Message reject = commproto::device::DeviceAuthRejectSerializer::serialize(std::move(commproto::device::DeviceAuthReject(rejectId, name)));
	stream->sendBytes(reject);
}

void AuthServiceImpl::sendPong()
{
	commproto::Message reject = commproto::device::KeepAliveSerializer::serialize(std::move(commproto::device::KeepAlive(keepAliveId)));
	stream->sendBytes(reject);
}

void AuthServiceImpl::scanStarted(const uint32_t scanAmount)
{
}

void AuthServiceImpl::scanProgress(const uint32_t complete)
{
}
