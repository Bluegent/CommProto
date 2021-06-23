#include "PlantHandlers.h"

SoilHandler::SoilHandler(const SinglePTrackerHandle& tracker_)
: tracker(tracker_)
{
}



void SoilHandler::handle(messages::MessageBase&& data)
{
	auto msg = static_cast<plant::Soil&>(data);
	tracker->setValue(msg.prop);
}

UvHandler::UvHandler(const SinglePTrackerHandle& tracker_)
	: tracker(tracker_)
{
}



void UvHandler::handle(messages::MessageBase&& data)
{
	auto msg = static_cast<plant::UvLight&>(data);
	tracker->setValue(msg.prop);
}

DHTHandler::DHTHandler(const InputHelperHandle& helper)
	:helper(helper)
{
}

void DHTHandler::handle(messages::MessageBase&& data)
{
	auto msg = static_cast<plant::TempHum&>(data);
	if(!helper)
	{
		return;
	}

	helper->temperature->setValue(msg.prop);
	helper->humidity->setValue(msg.prop2);
}
