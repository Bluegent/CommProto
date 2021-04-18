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
			virtual void setup() = 0;
			virtual void loop() = 0;
			virtual int32_t getTemp() = 0;
			virtual int32_t getHumidity() = 0;
			virtual void toggleHeating(const bool on) = 0;
			virtual void toggleCooling(const bool on) = 0;
			virtual void toggleAutoTempAdjust(const bool on) = 0;
			virtual void setDesiredTemp(const int32_t temp) = 0;
		};
	}
}

#endif // THERMO_WRAPPER_H