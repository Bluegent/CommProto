#include "AuthServiceHandlers.h"
#include <commproto/logger/Logging.h>

DeviceReqHandler::DeviceReqHandler(const AuthServiceHandle& service_)
	: service{ service_ }
{
}

void DeviceReqHandler::handle(commproto::messages::MessageBase&& data)
{
	commproto::device::DeviceAuthRequestMessage& msg = static_cast<commproto::device::DeviceAuthRequestMessage&>(data);
	if (msg.prop.size() != 4)
	{
		LOG_ERROR("Incomplete amount of request parameters");
	}
	APData ap{ msg.prop[0],msg.prop[1],msg.prop[2],msg.prop[3] };
	service->handleRequest(ap);
}


ScanFinishedHandler::ScanFinishedHandler(const AuthServiceHandle& service_)
	: service{ service_ }
{
}


void ScanFinishedHandler::handle(commproto::messages::MessageBase&& data)
{
	service->setScanFinished();
}

KeepAliveHandler::KeepAliveHandler(const AuthServiceHandle& service_)
	: service{ service_ }
{
}

void KeepAliveHandler::handle(commproto::messages::MessageBase&& data)
{
	if (!service)
	{
		return;
	}
	service->sendPong();
}

void ScanStartHandler::handle(commproto::messages::MessageBase&& data)
{
	commproto::device::ScanStarted & msg = static_cast<commproto::device::ScanStarted&>(data);
	if(!service)
	{
		return;
	}
	service->scanStarted(msg.prop);
}

void ScanProgressHandler::handle(commproto::messages::MessageBase&& data)
{
	commproto::device::ScanProgress & msg = static_cast<commproto::device::ScanProgress&>(data);
	if (!service)
	{
		return;
	}
	service->scanStarted(msg.prop);
}
