#ifndef PERCENTAGE_SINGLE_HEALTH_TRACKER_H
#define PERCENTAGE_SINGLE_HEALTH_TRACKER_H

#include <commproto/control/endpoint/UIFactory.h>
#include "Numbers.h"

using namespace commproto;


using HealthTrackerAction= std::function<void()>;

class PercentageSingleHealthTracker
{

public:

	void setValue(const uint32_t value);

	void updateLabels();

	void calibrateMin(const float value);

	void calibrateMax(const float value);


	void setDesiredMin(const float value);


	void setDesiredMax(const float value);

	void toggleCalibrationF(const bool state) const;

	void setOnLower(const HealthTrackerAction& lower);
	void setOnHigher(const HealthTrackerAction& higher);
	void setOnDesired(const HealthTrackerAction& desired);

	PercentageSingleHealthTracker(control::endpoint::UIFactory& factory, const std::string& name, const PercentageSensorTracker& tracker_, const Interval<uint32_t> & initiakValues, const std::string & solution);


private:
	control::endpoint::UIControllerHandle controller;
	control::endpoint::ToggleHandle toggleCalibration;
	control::endpoint::SliderHandle minCalibration;
	control::endpoint::SliderHandle maxCalibration;

	control::endpoint::SliderHandle minSlider;
	control::endpoint::SliderHandle maxSlider;

	control::endpoint::LabelHandle valueLabel;
	control::endpoint::LabelHandle scoreLabel;
	control::endpoint::LabelHandle solutionLabel;

	PercentageSensorTracker tracker;
	HealthTrackerAction onLower;
	HealthTrackerAction onHigher;
	HealthTrackerAction onDesired;
	bool wasDesired;
	bool wasLower;
	bool wasHigher;
};

#endif