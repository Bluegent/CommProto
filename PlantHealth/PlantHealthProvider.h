#ifndef PLANT_HEALTH_PROVIDER_H
#define PLANT_HEALTH_PROVIDER_H

#include <commproto/parser/ParserDelegator.h>
#include <commproto/endpoint/DelegatorProvider.h>

#include "PercentageSingleHealthTracker.h"
#include "OutputHelper.h"
#include "SingleHealthTracker.h"


using namespace commproto;

class InputHelper
{
public:
	explicit InputHelper(const SinglePTrackerHandle& soil_tracker, const SinglePTrackerHandle& uv_tracker, const SingleTrackerHandle & temperature, const SingleTrackerHandle & humidity, const SettingsHelperHandle & helper)
		: soilTracker(soil_tracker)
		, uvTracker(uv_tracker)
		, temperature(temperature)
		, humidity(humidity)
		, helper(helper)
	{
	}

	SinglePTrackerHandle soilTracker;
	SinglePTrackerHandle uvTracker;
	SingleTrackerHandle temperature;
	SingleTrackerHandle humidity;
	SettingsHelperHandle helper;

	uint32_t getOverallHealth() const;
};

using InputHelperHandle = std::shared_ptr<InputHelper>;


parser::ParserDelegatorHandle buildSelfDelegator();


class PlantHealthProvider : public endpoint::DelegatorProvider {
public:
	PlantHealthProvider(const messages::TypeMapperHandle & mapper_, const control::endpoint::UIControllerHandle & controller_, const InputHelperHandle & helper_, const OutputHelperHandle & outputHelper);

	parser::ParserDelegatorHandle provide(const std::string& name, const uint32_t id) override;
private:
	messages::TypeMapperHandle mapper;
	control::endpoint::UIControllerHandle controller;
	InputHelperHandle helper;
	OutputHelperHandle outHelper;
};

#endif