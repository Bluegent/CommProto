#ifndef SETTINGS_HELPER_H
#define SETTINGS_HELPER_H
#include <memory>
#include <functional>


template <typename T>
struct PlantStat
{

	T calibrateMin;
	T calibrateMax;
	float desireMin;
	float desireMax;
};

using IntPlantStat = PlantStat<uint32_t>;
using FloatPlantStat = PlantStat<float>;


namespace PlantDataNames
{
	constexpr const char * const soil = "soil";
	constexpr const char * const uv = "uv";
	constexpr const char * const humidity = "humidity";
	constexpr const char * const temperature = "temperature";
	
	constexpr const char * const calibrateMin = "calibrate_min";
	constexpr const char * const calibrateMax = "calibrate_max";
	constexpr const char * const desiredMin = "desired_min";
	constexpr const char * const desiredMax = "desired_max";
};

struct PlantData
{
	IntPlantStat soil;
	IntPlantStat uv;
	FloatPlantStat humidity;
	FloatPlantStat temperature;
	static PlantData getDefault();
};


using FSettingUpdate = std::function<void(const float)>;
using ISettingUpdate = std::function<void(const uint32_t)>;

class SettingsHelper
{
public:
	SettingsHelper(const std::string & path );
	bool load();
	void save() const;
	PlantData data;
	std::string path;
};

using SettingsHelperHandle = std::shared_ptr<SettingsHelper>;


#endif