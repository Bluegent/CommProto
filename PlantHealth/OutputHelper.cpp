#include "OutputHelper.h"
#include <commproto/service/ServiceChains.h>
#include <plant/interface/PlantMessages.h>

OutputHelper::OutputHelper(const sockets::SocketHandle& socket, const std::string & name)
	: socket(socket)
	, targetName(name)
	, startIrrigationId(0)
	, startUvId(0)
	, sendToId(0)
	, targetEpId(0)
{
}

void OutputHelper::startUv() const
{
	if (targetEpId == 0)
	{
		return;
	}
	auto msg = plant::ToggleUVLampSerializer::serialize(plant::ToggleUVLamp(startUvId, true));
	auto sendTo = service::SendtoSerializer::serialize(service::SendToMessage(sendToId,targetEpId,msg));
	socket->sendBytes(sendTo);
}

void OutputHelper::stopUv() const
{
	if (targetEpId == 0)
	{
		return;
	}
	auto msg = plant::ToggleUVLampSerializer::serialize(plant::ToggleUVLamp(startUvId, false));
	auto sendTo = service::SendtoSerializer::serialize(service::SendToMessage(sendToId, targetEpId, msg));
	socket->sendBytes(sendTo);
}

void OutputHelper::startPump() const
{
	if (targetEpId == 0)
	{
		return;
	}
	auto msg = plant::TogglePumpSerializer::serialize(plant::TogglePump(startIrrigationId, true));
	auto sendTo = service::SendtoSerializer::serialize(service::SendToMessage(sendToId, targetEpId, msg));
	socket->sendBytes(sendTo);
}

void OutputHelper::stopPump() const
{
	if (targetEpId == 0)
	{
		return;
	}
	auto msg = plant::TogglePumpSerializer::serialize(plant::TogglePump(startIrrigationId, false));
	auto sendTo = service::SendtoSerializer::serialize(service::SendToMessage(sendToId, targetEpId, msg));
	socket->sendBytes(sendTo);
}

void OutputHelper::setIds(const uint32_t soil, const uint32_t uv, const uint32_t sendTo)
{
	startIrrigationId = soil;
	startUvId = uv;
	sendToId = sendTo;
}



void OutputHelper::notifyMapping(const std::string& name, const uint32_t id)
{
	if (targetName != name)
		return;
	targetEpId = id;
}
