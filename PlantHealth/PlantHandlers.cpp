#include "PlantHandlers.h"

SoilHandler::SoilHandler(PercentageSingleHealthTracker& tracker_)
: tracker(tracker_)
{
}



void SoilHandler::handle(messages::MessageBase&& data)
{
	auto msg = static_cast<plant::Soil&>(data);
	tracker.setValue(msg.prop);
}

UvHandler::UvHandler(PercentageSingleHealthTracker& tracker_)
	: tracker(tracker_)
{
}



void UvHandler::handle(messages::MessageBase&& data)
{
	auto msg = static_cast<plant::UvLight&>(data);
	tracker.setValue(msg.prop);
}