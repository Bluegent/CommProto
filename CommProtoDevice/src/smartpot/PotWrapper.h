#ifndef  SMARTPOT_WRAPPER_H
#define  SMARTPOT_WRAPPER_H
#include <cstdint>


class PotWrapper
{
public:
	virtual ~PotWrapper() = default;
	virtual float getTemperature() = 0;
	virtual float getHumidity() = 0;
	virtual uint32_t getSoilHumidity() = 0;
	virtual uint32_t getLightExposure() = 0;
	virtual void togglePump(bool state) = 0;
	virtual void toggleLamp(bool state) = 0;
	virtual void setupBoard() = 0;
	virtual uint32_t getMs() = 0;
	virtual void getReadings() = 0;
	virtual void loop() = 0;
};



#endif