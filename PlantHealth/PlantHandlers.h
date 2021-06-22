#ifndef PLANT_HANDLERS_H
#define PLANT_HANDLERS_H

#include <commproto/parser/Handler.h>
#include <PercentageSingleHealthTracker.h>
#include <plant/interface/PlantMessages.h>

using namespace commproto;
class SoilHandler : public parser::Handler
{
public:
	SoilHandler(PercentageSingleHealthTracker& tracker_);
	void handle(messages::MessageBase&& data) override;
private:
	PercentageSingleHealthTracker & tracker;
};




#endif