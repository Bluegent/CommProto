#ifndef PLANT_HEALTH_PROVIDER_H
#define PLANT_HEALTH_PROVIDER_H

#include <commproto/parser/ParserDelegator.h>
#include <commproto/endpoint/DelegatorProvider.h>

#include "PercentageSingleHealthTracker.h"



using namespace commproto;

class InputHelper
{
public:
	explicit InputHelper(PercentageSingleHealthTracker& soil_tracker, PercentageSingleHealthTracker& uv_tracker)
		: soilTracker(soil_tracker)
		, uvTracker(uv_tracker)
	{
	}

	PercentageSingleHealthTracker soilTracker;
	PercentageSingleHealthTracker uvTracker;
};

using InputHelperHandle = std::shared_ptr<InputHelper>;


parser::ParserDelegatorHandle buildSelfDelegator();


class PlantHealthProvider : public endpoint::DelegatorProvider {
public:
	PlantHealthProvider(const messages::TypeMapperHandle & mapper_, const control::endpoint::UIControllerHandle & controller_, const InputHelperHandle & helper_);

	parser::ParserDelegatorHandle provide(const std::string& name, const uint32_t id) override;
private:
	messages::TypeMapperHandle mapper;
	control::endpoint::UIControllerHandle controller;
	InputHelperHandle helper;
};

#endif