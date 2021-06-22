#include "PercentageSingleHealthTracker.h"
#include <sstream>


void PercentageSingleHealthTracker::setValue(const uint32_t value)
{
	tracker.value.setValue(value);
	updateLabels();
}

void PercentageSingleHealthTracker::updateLabels()
{
	tracker.value.calcPercentage();

	if (scoreLabel)
	{
		uint32_t score = tracker.getScore();
		std::stringstream scoreStream;
		scoreStream << score;
		scoreLabel->setText(scoreStream.str());
	}

	if (valueLabel)
	{
		float percentage = tracker.value.getPercentage();
		std::stringstream valueStream;
		valueStream.precision(3);
		valueStream << percentage << "%(" << tracker.value.getValue() << ")";
		valueLabel->setText(valueStream.str());
	}
}

void PercentageSingleHealthTracker::calibrateMin(const float value)
{
	tracker.total.left = static_cast<uint32_t>(value);
	updateLabels();
}

void PercentageSingleHealthTracker::calibrateMax(const float value)
{
	tracker.total.right = static_cast<uint32_t>(value);
	updateLabels();
}

void PercentageSingleHealthTracker::setDesiredMin(const float value)
{
	tracker.desired.left = value;
	updateLabels();
}

void PercentageSingleHealthTracker::setDesiredMax(const float value)
{
	tracker.desired.right = value;
	updateLabels();
}

void PercentageSingleHealthTracker::toggleCalibrationF(const bool state) const
{
	if (minCalibration)
	{
		controller->setControlShownState(minCalibration->getId(), state);
	}
	if (maxCalibration)
	{
		controller->setControlShownState(maxCalibration->getId(), state);
	}
}

PercentageSingleHealthTracker::PercentageSingleHealthTracker(commproto::control::endpoint::UIFactory& factory, const std::string& name, const PercentageSensorTracker& tracker_): tracker(tracker_)
{
	commproto::control::endpoint::ToggleAction toggleCalibrationAct = [&, this](bool state)
	{
		this->toggleCalibrationF(state);
	};

	toggleCalibration = factory.makeToggle(name + " Calibration", toggleCalibrationAct);


	commproto::control::endpoint::SliderAction calibrateMinAct = [&, this](float value)
	{
		this->calibrateMin(value);
	};
	minCalibration = factory.makeSlider(name + " - Calibrate Min", calibrateMinAct);
	minCalibration->setInitialValue(tracker.value.absolute.left);
	minCalibration->setLimits(tracker.value.absolute.left, tracker.value.absolute.right);
	minCalibration->setStep(1);
	minCalibration->setDisplayState(false);


	control::endpoint::SliderAction calibrateMaxAct = [&, this](float value)
	{
		this->calibrateMax(value);
	};
	maxCalibration = factory.makeSlider(name + " - Calibrate Max", calibrateMaxAct);
	maxCalibration->setInitialValue(tracker.value.absolute.right);
	maxCalibration->setLimits(tracker.value.absolute.left, tracker.value.absolute.right);
	maxCalibration->setStep(1);
	maxCalibration->setDisplayState(false);


	control::endpoint::SliderAction desiredMinAct = [&, this](float value)
	{
		this->setDesiredMin(value);
	};
	minSlider = factory.makeSlider("Desired Min " + name, desiredMinAct, "%");
	minSlider->setInitialValue(tracker.desired.left);
	minSlider->setLimits(0, 100);
	minSlider->setStep(.5);


	control::endpoint::SliderAction desiredMaxAct = [&, this](float value)
	{
		this->setDesiredMax(value);
	};
	maxSlider = factory.makeSlider("Desired Max " + name, desiredMaxAct, "%");
	maxSlider->setInitialValue(tracker.desired.right);
	maxSlider->setLimits(0, 100);
	maxSlider->setStep(.5);

	valueLabel = factory.makeLabel(name, "");
	scoreLabel = factory.makeLabel(name + " Score", "");
	updateLabels();


	controller = factory.makeController();
	controller->addControl(toggleCalibration);
	controller->addControl(minCalibration);
	controller->addControl(maxCalibration);
	controller->addControl(minSlider);
	controller->addControl(maxSlider);
	controller->addControl(valueLabel);
	controller->addControl(scoreLabel);
}
