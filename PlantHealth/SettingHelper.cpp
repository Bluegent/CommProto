#include "SettingHelper.h"
#include <fstream>
#include "commproto/logger/Logging.h"
#include <Poco/JSON/JSONException.h>
#include <Poco/JSON/Object.h>
#include <Poco/JSON/Parser.h>

template <typename T>
void initialize(PlantStat<T> & stat, T min, T max, float dMin, float dMax,float coeff)
{
	stat.calibrateMin = min;
	stat.calibrateMax = max;
	stat.desireMin = dMin;
	stat.desireMax = dMax;
	stat.coeff = coeff;
}

PlantData PlantData::getDefault()
{
	PlantData data;
	initialize<float>(data.humidity, 0, 100.0, 40.0f, 60.0f,0.25);
	initialize<float>(data.temperature, 5, 50.0, 20.f, 35.0f,0.25);
	initialize<uint32_t>(data.soil, 1200, 3500, 40.0f, 60.0f,0.40);
	initialize<uint32_t>(data.uv, 0, 200, 40.0f, 100.0f,0.10);

	return data;
}

SettingsHelper::SettingsHelper(const std::string & path)
	: data(PlantData::getDefault())
	, path(path)
{
}

template <typename T>
void readStat(PlantStat<T> & stat, const Poco::JSON::Object::Ptr & obj)
{
	stat.calibrateMin = obj->get(PlantDataNames::calibrateMin).convert<T>();
	stat.calibrateMax = obj->get(PlantDataNames::calibrateMax).convert<T>();
	stat.desireMin = obj->get(PlantDataNames::desiredMin).convert<float>();
	stat.desireMax = obj->get(PlantDataNames::desiredMax).convert<float>();
	stat.coeff = obj->get(PlantDataNames::coeff).convert<float>();
}

bool SettingsHelper::load()
{
	std::ifstream file(path);
	if (!file.is_open())
	{
		LOG_ERROR("Could not load plant data from file \"%s\"", path);
		return false;
	}

	std::stringstream fileStream;

	while (!file.eof())
	{
		std::string line;
		std::getline(file, line);
		fileStream << line;
	}
	if (fileStream.str().empty())
	{
		LOG_ERROR("Could not load plant data from empty file \"%s\"", path);
		return false;
	}


	Poco::JSON::Parser parser;
	Poco::Dynamic::Var result = parser.parse(fileStream.str());
	Poco::JSON::Object::Ptr root = result.extract<Poco::JSON::Object::Ptr>();


	Poco::JSON::Object::Ptr soil = root->getObject(PlantDataNames::soil);
	readStat(data.soil, soil);

	Poco::JSON::Object::Ptr uv = root->getObject(PlantDataNames::uv);
	readStat(data.uv, uv);

	Poco::JSON::Object::Ptr temperature = root->getObject(PlantDataNames::temperature);
	readStat(data.temperature, temperature);

	Poco::JSON::Object::Ptr humidity = root->getObject(PlantDataNames::humidity);
	readStat(data.humidity, humidity);

	return true;
}


template <typename T>
void writeStat(const PlantStat<T> & stat, Poco::JSON::Object & obj)
{
	obj.set(PlantDataNames::calibrateMin, stat.calibrateMin);
	obj.set(PlantDataNames::calibrateMax, stat.calibrateMax);
	obj.set(PlantDataNames::desiredMin, stat.desireMin);
	obj.set(PlantDataNames::desiredMax, stat.desireMax);
	obj.set(PlantDataNames::coeff, stat.coeff);
}


void SettingsHelper::save() const
{
	std::ofstream file(path);
	if (!file.is_open())
	{
		LOG_ERROR("Could not save plant data to file \"%s\"", path);
		return;
	}

	Poco::JSON::Object root;

	Poco::JSON::Object soil;
	writeStat<uint32_t>(data.soil, soil);


	Poco::JSON::Object uv;
	writeStat<uint32_t>(data.uv, uv);

	Poco::JSON::Object humidity;
	writeStat<float>(data.humidity, humidity);

	Poco::JSON::Object temperature;
	writeStat<float>(data.temperature, temperature);


	root.set(PlantDataNames::soil, soil);
	root.set(PlantDataNames::uv, uv);
	root.set(PlantDataNames::humidity, humidity);
	root.set(PlantDataNames::temperature, temperature);


	std::stringstream rootStream;
	root.stringify(rootStream, 1);

	file << rootStream.str();
	file.close();
}
