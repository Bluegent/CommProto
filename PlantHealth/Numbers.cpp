#include "Numbers.h"
#include <commproto/logger/Logging.h>

AbsoluteToPercentage::AbsoluteToPercentage(const Interval<uint32_t>& absolute)
	: absolute(absolute)
	, value(0)
	, percentage(0.f)
{
}

void AbsoluteToPercentage::setValue(const uint32_t value_)
{
	value = value_;
	clamp(value, absolute.left, absolute.right);
	calcPercentage();

}

float AbsoluteToPercentage::getPercentage() const
{
	return percentage;

}

void AbsoluteToPercentage::calcPercentage()
{
	float left = static_cast<float>(absolute.left);
	float right = static_cast<float>(absolute.right);
	float cValue = static_cast<float>(value);
	percentage = (cValue - left) / (right - left) * 100.f;
}

PercentageSensorTracker::PercentageSensorTracker(const AbsoluteToPercentage& value, const Interval<float>& desired, const Interval<float>& total)
	: desired(desired)
	, total(total)
	, value(value)
{
}

uint32_t PercentageSensorTracker::getScore()
{
	float percentage = value.getPercentage();
	if (desired.contains(percentage))
	{
		return 100;
	}
	if (percentage < desired.left && percentage >= total.left)
	{
		return static_cast<uint32_t>((percentage - total.left) / (desired.left - total.left)*100.f);
	}

	if (percentage > desired.right && percentage <= total.right)
	{
		return static_cast<uint32_t>((total.right - percentage) / (total.right - desired.right)*100.f);
	}

	return 0;
}


void testTracker(SensorTracker<float> & tracker, float value1, float value2, float value3)
{
	tracker.value = value1;
	uint32_t score1 = tracker.getScore();
	tracker.value = value2;
	uint32_t score2 = tracker.getScore();
	tracker.value = value3;
	uint32_t score3 = tracker.getScore();

	LOG_INFO("Scores: %.2f - %.2f(%d) %.2f(%d)  %.2f(%d) - %.2f", tracker.total.left, value1, score1, value2, score2, value3, score3, tracker.total.right);
}

void testNumbers()
{
	PercentageSensorTracker soil(AbsoluteToPercentage(Interval<uint32_t>(0, 4096)), Interval<float>(40, 60), Interval<float>(20, 100));

	soil.value.setValue(2500);
	uint32_t scoreTwoThou = soil.getScore();
	soil.value.setValue(100);
	uint32_t scoreOneHun = soil.getScore();
	soil.value.setValue(3000);
	uint32_t scoreThreeThou = soil.getScore();

	LOG_INFO("Scores: 0- 100(%d) 2500(%d)  3000(%d) - 4096", scoreOneHun, scoreTwoThou, scoreThreeThou);

	SensorTracker<float> humidity(Interval<float>(40, 60), Interval<float>(0, 100));
	testTracker(humidity, 10, 66, 99);


	SensorTracker<float> temperature(Interval<float>(20, 24), Interval<float>(15, 40));
	testTracker(temperature, 13, -3, 66);

}

