#ifndef ROTARY_CHAINS_H
#define ROTARY_CHAINS_H
#include <commproto/messages/TriplePropertyChain.h>
#include <commproto/messages/DoublePropertyChain.h>

namespace rotary
{

	namespace endpoint
	{
		//float order;
		//left limit, right limit, initial value, step
		MAKE_TRIPLE_PROP_CHAIN(RotaryMessage, uint32_t, std::vector<std::string>, std::vector<float>);
	}
	namespace ux
	{
		MAKE_DOUBLE_PROP_CHAIN(RotaryAdjust, uint32_t, float);
	}
}

namespace commproto
{
	DEFINE_DATA_TYPE(rotary::endpoint::RotaryMessage);
	DEFINE_DATA_TYPE(rotary::ux::RotaryAdjust);
}


#endif //ROTARY_CHAINS_H