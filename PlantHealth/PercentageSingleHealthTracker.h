#ifndef PERCENTAGE_SINGLE_HEALTH_TRACKER_H
#define PERCENTAGE_SINGLE_HEALTH_TRACKER_H

#include <commproto/control/endpoint/UIFactory.h>
#include "Numbers.h"
#include "SettingHelper.h"

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
	void setUpdates(const ISettingUpdate & min, const ISettingUpdate & max, const FSettingUpdate & desireMin, const FSettingUpdate & desireMax);
	void setOvearll(const OverallUpdate& update);

	PercentageSingleHealthTracker(control::endpoint::UIFactory& factory, const std::string& name, const PercentageSensorTracker& tracker_, const Interval<uint32_t> & initiakValues, const std::string & solution);
	PercentageSensorTracker getTracker() const;

private:

	void executeOnDesired();
	void executeOnLower();
	void executeOnHigher();


	control::endpoint::UIControllerHandle controller;
	control::endpoint::ToggleHandle toggleCalibration;
	control::endpoint::SliderHandle minCalibration;
	control::endpoint::SliderHandle maxCalibration;

	control::endpoint::SliderHandle minSlider;
	control::endpoint::SliderHandle maxSlider;

	control::endpoint::LabelHandle valueLabel;
	control::endpoint::ProgressBarHandle scoreBar;


	control::endpoint::ToggleHandle toggleAuto;
	control::endpoint::LabelHandle solutionLabel;

	PercentageSensorTracker tracker;
	HealthTrackerAction onLower;
	HealthTrackerAction onHigher;
	HealthTrackerAction onDesired;
	bool wasDesired;
	bool wasLower;
	bool wasHigher;
	bool enabledAuto;
	ISettingUpdate onCalibrateMin;
	ISettingUpdate onCalibrateMax;
	FSettingUpdate onDesireMin;
	FSettingUpdate onDesireMax;
	OverallUpdate overall;
};


using SinglePTrackerHandle = std::shared_ptr<PercentageSingleHealthTracker>;

#endif