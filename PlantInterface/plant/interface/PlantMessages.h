#ifndef PLANT_MESSAGES_H
#define PLANT_MESSAGES_H

#include <commproto/messages/DoublePropertyChain.h>
#include <commproto/messages/SinglePropertyChain.h>

namespace plant
{

	//sensor values

	// DHT reading, temperature, than humidity
	// temperature is in celsius, humidity is in percentage
	MAKE_DOUBLE_PROP_CHAIN(TempHum,float,float);

	// Soil humidity sensor 0 - 4096
	MAKE_SINGLE_PROP_CHAIN(Soil, uint32_t);

	// UV Light 0 - 4096
	MAKE_SINGLE_PROP_CHAIN(UvLight,uint32_t);

	//actuator messages

	//toggles pump control, true - irrigate, false - stop
	MAKE_SINGLE_PROP_CHAIN(TogglePump, bool);

	//toggles UV lamp, true - on, false - off
	MAKE_SINGLE_PROP_CHAIN(ToggleUVLamp, bool);

}

namespace commproto
{
	DEFINE_DATA_TYPE(plant::TempHum);
	DEFINE_DATA_TYPE(plant::Soil);
	DEFINE_DATA_TYPE(plant::UvLight);
	DEFINE_DATA_TYPE(plant::TogglePump);
	DEFINE_DATA_TYPE(plant::ToggleUVLamp);
}

#endif