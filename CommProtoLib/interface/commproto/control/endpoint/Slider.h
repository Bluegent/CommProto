#ifndef ENDPOINT_SLIDER_H
#define ENDPOINT_SLIDER_H
#include <commproto/control/endpoint/Control.h>
#include <functional>

namespace commproto
{
	namespace control
	{
		namespace endpoint
		{

			using SliderAction = std::function<void(float)>;

			class Slider : public Control {
			public:
				Slider(const std::string & name, const uint32_t id)
					: Control{ name,id }
				{

				}
				virtual float getValue() const = 0;
				//triggers the slider action
				virtual void setValue(const float value_) = 0;
				virtual void setLimits(const float left_, const float right_) = 0;
				virtual void setInitialValue(const float value_) = 0;
				virtual void setStep(const float step_) = 0;
			};

			using SliderHandle = std::shared_ptr<Slider>;
		}
	}
}


#endif // SLIDER_H