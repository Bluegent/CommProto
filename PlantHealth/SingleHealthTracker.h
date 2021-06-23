#ifndef SINGLE_HEALTH_TRACKER_H
#define SINGLE_HEALTH_TRACKER_H

#include <commproto/control/endpoint/UIFactory.h>
#include "Numbers.h"
#include "SettingHelper.h"

using namespace commproto;


using HealthTrackerAction= std::function<void()>;

class SingleHealthTracker
{

public:

	void setValue(const float value);

	void updateLabels();

	void setDesiredMin(const float value);


	void setDesiredMax(const float value);

	void toggleCalibrationF(const bool state) const;

	void setUpdates( const FSettingUpdate & desireMin, const FSettingUpdate & desireMax);


	SingleHealthTracker(control::endpoint::UIFactory& factory, const std::string& name, const SensorTracker<float>& tracker_, const std::string & unit);
	SensorTracker<float> getTracker() const;
	void setOvearll(const OverallUpdate& update);

private:


	control::endpoint::UIControllerHandle controller;
	control::endpoint::ToggleHandle toggleCalibration;

	control::endpoint::SliderHandle minSlider;
	control::endpoint::SliderHandle maxSlider;

	control::endpoint::LabelHandle valueLabel;
	control::endpoint::ProgressBarHandle scoreBar;


	std::string unitOfMeasure;
	SensorTracker<float> tracker;
	FSettingUpdate onDesireMin;
	FSettingUpdate onDesireMax; 
	OverallUpdate overall;
};


using SingleTrackerHandle = std::shared_ptr<SingleHealthTracker>;

#endif