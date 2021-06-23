#ifndef PLANT_HANDLERS_H
#define PLANT_HANDLERS_H

#include <commproto/parser/Handler.h>
#include <PercentageSingleHealthTracker.h>
#include <plant/interface/PlantMessages.h>
#include "PlantHealthProvider.h"

using namespace commproto;
class SoilHandler : public parser::Handler
{
public:
	SoilHandler(const SinglePTrackerHandle& tracker_);
	void handle(messages::MessageBase&& data) override;
private:
	SinglePTrackerHandle tracker;
};

class UvHandler : public parser::Handler
{
public:
	UvHandler(const SinglePTrackerHandle& tracker_);
	void handle(messages::MessageBase&& data) override;
private:
	SinglePTrackerHandle tracker;
};

class DHTHandler : public parser::Handler
{
public:
	DHTHandler(const InputHelperHandle& helper);
	void handle(messages::MessageBase&& data) override;
private:
	InputHelperHandle helper;
};


#endif