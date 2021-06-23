#include "SingleHealthTracker.h"
#include <sstream>
#include "commproto/logger/Logging.h"


void SingleHealthTracker::setValue(const float value)
{
	tracker.setValue(value);
	float perc = tracker.getValue();
	updateLabels();
	overall();
}

void SingleHealthTracker::updateLabels()
{
	if (scoreBar)
	{
		uint32_t score = tracker.getScore();
		scoreBar->setProgress(score);
	}

	if (valueLabel)
	{
		float percentage = tracker.getValue();
		std::stringstream valueStream;
		valueStream.precision(3);
		valueStream << percentage << " " << unitOfMeasure;
		valueLabel->setText(valueStream.str());
	}
}

void SingleHealthTracker::setDesiredMin(const float value)
{
	tracker.desired.left = value;
	onDesireMin(value);
	updateLabels();
}

void SingleHealthTracker::setDesiredMax(const float value)
{
	tracker.desired.right = value;
	onDesireMax(value);
	updateLabels();
}

void SingleHealthTracker::toggleCalibrationF(const bool state) const
{
	if (minSlider)
	{
		controller->setControlShownState(minSlider->getId(), state);
	}
	if (maxSlider)
	{
		controller->setControlShownState(maxSlider->getId(), state);
	}
}

void SingleHealthTracker::setUpdates(const FSettingUpdate& desireMin, const FSettingUpdate& desireMax)
{
	onDesireMin = desireMin;
	onDesireMax = desireMax;
}



SingleHealthTracker::SingleHealthTracker(commproto::control::endpoint::UIFactory& factory, const std::string& name, const SensorTracker<float>& tracker_, const std::string & unit)
	: unitOfMeasure(unit)
	, tracker(tracker_)
{
	commproto::control::endpoint::ToggleAction toggleCalibrationAct = [&, this](bool state)
	{
		this->toggleCalibrationF(state);
	};

	toggleCalibration = factory.makeToggle(name + " Calibration", toggleCalibrationAct);

	control::endpoint::SliderAction desiredMinAct = [&, this](float value)
	{
		this->setDesiredMin(value);
	};
	minSlider = factory.makeSlider("Desired Min " + name, desiredMinAct, unitOfMeasure);
	minSlider->setInitialValue(tracker.desired.left);
	minSlider->setLimits(tracker.total.left,tracker.total.right);
	minSlider->setStep(.5);
	minSlider->setDisplayState(false);


	control::endpoint::SliderAction desiredMaxAct = [&, this](float value)
	{
		this->setDesiredMax(value);
	};
	maxSlider = factory.makeSlider("Desired Max " + name, desiredMaxAct, unitOfMeasure);
	maxSlider->setInitialValue(tracker.desired.right);
	maxSlider->setLimits(tracker.total.left, tracker.total.right);
	maxSlider->setStep(.5);
	maxSlider->setDisplayState(false);

	valueLabel = factory.makeLabel(name, "");
	scoreBar = factory.makeProgresBar(name + " Health Score", 0);

	updateLabels();


	controller = factory.makeController();
	controller->addControl(toggleCalibration);
	controller->addControl(minSlider);
	controller->addControl(maxSlider);
	controller->addControl(valueLabel);
	controller->addControl(scoreBar);

	updateLabels();
}

SensorTracker<float> SingleHealthTracker::getTracker() const
{
	return tracker;
}

void SingleHealthTracker::setOvearll(const OverallUpdate& update)
{
	overall = update;
}
