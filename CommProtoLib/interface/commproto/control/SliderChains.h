#ifndef SLIDER_CHAINS_H
#define SLIDER_CHAINS_H
#include <commproto/messages/TriplePropertyChain.h>
#include <commproto/messages/DoublePropertyChain.h>

namespace commproto
{
	namespace control
	{
		namespace endpoint 
		{
			//float order;
			//left limit, right limit, initial value, step
			MAKE_TRIPLE_PROP_CHAIN(SliderMessage, uint32_t, std::string, std::vector<float>);
		}
		namespace ux
		{
			MAKE_DOUBLE_PROP_CHAIN(SliderAdjust, uint32_t, float);
		}
	}


	DEFINE_DATA_TYPE(control::endpoint::SliderMessage);
	DEFINE_DATA_TYPE(control::ux::SliderAdjust);

}


#endif// ENDPOINT_SLIDER_CHAINS_H