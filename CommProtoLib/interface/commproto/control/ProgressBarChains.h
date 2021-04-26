#ifndef PROGRESS_BAR_CHAINS_H
#define PROGRESS_BAR_CHAINS_H
#include <commproto/messages/TriplePropertyChain.h>
#include <commproto/messages/DoublePropertyChain.h>

namespace commproto
{
	namespace control
	{
		namespace endpoint 
		{
			// id, name, start progress
			MAKE_TRIPLE_PROP_CHAIN(ProgressBarMessage, uint32_t, std::string, uint32_t);

			//id, current progress
			MAKE_DOUBLE_PROP_CHAIN(ProgressBarUpdate, uint32_t, uint32_t);
		}
	}


	DEFINE_DATA_TYPE(control::endpoint::ProgressBarMessage);
	DEFINE_DATA_TYPE(control::endpoint::ProgressBarUpdate);
}


#endif// PROGRESS_BAR_CHAINS_H