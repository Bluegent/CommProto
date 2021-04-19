#ifndef THERMO_WRAPPER_H
#define THERMO_WRAPPER_H
#include <stdint.h>

namespace commproto
{
	namespace thermo
	{
		class ThermostateWrapper
		{
		public:
			virtual ~ThermostateWrapper() = default;
			virtual uint32_t getMs() = 0;
			virtual void setup() = 0;
			virtual void loop() = 0;
			virtual float getTemp() = 0;
			virtual float getHumidity() = 0;
			virtual void toggleTempAdjust(float intensity) = 0;
			virtual void toggleAutoTempAdjust(const bool on) = 0;
			virtual void setDesiredTemp(const float temp) = 0;
		};
	}
}

#endif // THERMO_WRAPPER_H