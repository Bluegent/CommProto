#include "PercentageSingleHealthTracker.h"
#include <sstream>
#include "commproto/logger/Logging.h"


void PercentageSingleHealthTracker::setValue(const uint32_t value)
{
	tracker.value.setValue(value);
	tracker.value.calcPercentage();
	float perc = tracker.value.getPercentage();
	updateLabels();

	if(!enabledAuto)
	{
		return;
	}

	executeOnLower();
	executeOnDesired();
	executeOnHigher();
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
	tracker.value.absolute.left = static_cast<uint32_t>(value);
	onCalibrateMin(static_cast<uint32_t>(value));
	updateLabels();
}

void PercentageSingleHealthTracker::calibrateMax(const float value)
{
	tracker.value.absolute.right = static_cast<uint32_t>(value);
	onCalibrateMax(static_cast<uint32_t>(value));
	updateLabels();
}

void PercentageSingleHealthTracker::setDesiredMin(const float value)
{
	tracker.desired.left = value;
	onDesireMin(value);
	updateLabels();
}

void PercentageSingleHealthTracker::setDesiredMax(const float value)
{
	tracker.desired.right = value;
	onDesireMax(value);
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
	if (minSlider)
	{
		controller->setControlShownState(minSlider->getId(), state);
	}
	if (maxSlider)
	{
		controller->setControlShownState(maxSlider->getId(), state);
	}
}

void PercentageSingleHealthTracker::setOnLower(const HealthTrackerAction& lower)
{
	onLower = lower;
}

void PercentageSingleHealthTracker::setOnHigher(const HealthTrackerAction& higher)
{
	onHigher = higher;
}

void PercentageSingleHealthTracker::setOnDesired(const HealthTrackerAction& desired)
{
	onDesired = desired;
}

void PercentageSingleHealthTracker::setUpdates(const ISettingUpdate& min, const ISettingUpdate& max, const FSettingUpdate& desireMin, const FSettingUpdate& desireMax)
{
	onCalibrateMax = max;
	onCalibrateMin = min;
	onDesireMin = desireMin;
	onDesireMax = desireMax;
}



PercentageSingleHealthTracker::PercentageSingleHealthTracker(commproto::control::endpoint::UIFactory& factory, const std::string& name, const PercentageSensorTracker& tracker_, const Interval<uint32_t> & initiakValues, const std::string & solution)
	: tracker(tracker_)
	, wasDesired(false)
	, wasLower(false)
	, wasHigher(false)
	, enabledAuto(false)
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
	minCalibration->setInitialValue(initiakValues.left);
	minCalibration->setLimits(tracker.value.absolute.left, tracker.value.absolute.right);
	minCalibration->setStep(1);
	minCalibration->setDisplayState(false);


	control::endpoint::SliderAction calibrateMaxAct = [&, this](float value)
	{
		this->calibrateMax(value);
	};
	maxCalibration = factory.makeSlider(name + " - Calibrate Max", calibrateMaxAct);
	maxCalibration->setInitialValue(initiakValues.right);
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
	minSlider->setDisplayState(false);


	control::endpoint::SliderAction desiredMaxAct = [&, this](float value)
	{
		this->setDesiredMax(value);
	};
	maxSlider = factory.makeSlider("Desired Max " + name, desiredMaxAct, "%");
	maxSlider->setInitialValue(tracker.desired.right);
	maxSlider->setLimits(0, 100);
	maxSlider->setStep(.5);
	maxSlider->setDisplayState(false);

	valueLabel = factory.makeLabel(name, "");
	scoreLabel = factory.makeLabel(name + " Score", "");

	control::endpoint::ToggleAction autoAction = [&,this](const bool state)
	{
		LOG_INFO("%s auto solutioning",state?"Enabling":"Disabling");
		enabledAuto = state;
		if(enabledAuto)
		{
			setValue(tracker.value.getValue());
		}
		if(!enabledAuto)
		{
			solutionLabel->setText("Off");
			wasLower = false;
			wasHigher = false;
			wasDesired = true;
			if (onDesired) 
			{
				onDesired();
			}
		}
	};

	toggleAuto = factory.makeToggle("Enable auto-" + solution, autoAction);

	solutionLabel = factory.makeLabel(solution, "Off");

	updateLabels();


	controller = factory.makeController();
	controller->addControl(toggleCalibration);
	controller->addControl(minCalibration);
	controller->addControl(maxCalibration);
	controller->addControl(minSlider);
	controller->addControl(maxSlider);
	controller->addControl(valueLabel);
	controller->addControl(scoreLabel);
	controller->addControl(toggleAuto);
	controller->addControl(solutionLabel);

	tracker.value.absolute.left = static_cast<uint32_t>(initiakValues.left);
	tracker.value.absolute.right = static_cast<uint32_t>(initiakValues.right);
	updateLabels();
}

PercentageSensorTracker PercentageSingleHealthTracker::getTracker() const
{
	return tracker;
}

void PercentageSingleHealthTracker::executeOnDesired()
{
	float perc = tracker.value.getPercentage();
	if (tracker.desired.contains(perc) && !wasDesired)
	{
		wasLower = false;
		wasHigher = false;
		wasDesired = true;
		solutionLabel->setText("Off");
		if (onDesired) {
			onDesired();
		}
	}
}

void PercentageSingleHealthTracker::executeOnLower()
{
	float perc = tracker.value.getPercentage();
	if (perc < tracker.desired.left && !wasLower)
	{
		wasLower = true;
		wasHigher = false;
		wasDesired = false;
		if (onLower) {
			onLower();
			solutionLabel->setText("On");
		}
	}
}

void PercentageSingleHealthTracker::executeOnHigher()
{
	float perc = tracker.value.getPercentage();
	if (perc > tracker.desired.right && !wasHigher)
	{
		wasLower = false;
		wasHigher = true;
		wasDesired = false;
		if (onHigher) {
			onHigher();
			solutionLabel->setText("On");
		}
		return;
	}
}
