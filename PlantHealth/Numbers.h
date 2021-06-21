#ifndef PLANT_NUMBERS_H
#define PLANT_NUMBERS_H

#include <cstdint>

template <typename T>
void clamp(T& value, const T left, const T right)
{
	if (value < left)
	{
		value = left;
	}
	if (value > right)
	{
		value = right;
	}
}

template <typename T>
struct Interval
{
	T left;
	T right;
	Interval(const T left, const T right);
	bool contains(const T value);
	T getDistance(const T value);
};

template <typename T>
Interval<T>::Interval(const T left, const T right)
	: left(left)
	, right(right)
{
}

template <typename T>
bool Interval<T>::contains(const T value)
{
	return value >= left && right >= value;
}

template <typename T>
T Interval<T>::getDistance(const T value)
{
	if (value > right)
	{
		return value - right;
	}
	if (value < left)
	{
		return left - value;
	}
	return T{};
}

struct AbsoluteToPercentage
{
	const Interval<uint32_t> absolute;
	AbsoluteToPercentage(const Interval<uint32_t>& absolute);

	void setValue(const uint32_t value);
	float getPercentage() const;
private:
	void calcPercentage();
	uint32_t value;
	float percentage;

};


template <typename T>
struct SensorTracker
{
	Interval<T> desired;
	Interval<T> total;
	T value;
	SensorTracker(const Interval<T> & desired, const Interval<T> & total);
	bool isDesired();
	void setValue(const T type);
	uint32_t getScore();
};


struct PercentageSensorTracker
{
	PercentageSensorTracker(const AbsoluteToPercentage& value, const Interval<float>& desired, const Interval<float>& total = Interval<float>(0, 100));

	Interval<float> desired;
	Interval<float> total;
	AbsoluteToPercentage value;

	uint32_t getScore();
};

template <typename T>
SensorTracker<T>::SensorTracker(const Interval<T>& desired, const Interval<T> & total)
	: desired(desired)
	, total(total)
	, value{}
{
}

template <typename T>
bool SensorTracker<T>::isDesired()
{
	return desired.contains(value);
}

template <typename T>
void SensorTracker<T>::setValue(const T value_)
{
	value = value_;
}

template <typename T>
uint32_t SensorTracker<T>::getScore()
{
	float percentage = value;
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


void testNumbers();

#endif;
