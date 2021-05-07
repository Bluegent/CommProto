#ifndef COMMPROTO_DIAGNOSTICS_CHAINS_H
#define COMMPROTO_DIAGNOSTICS_CHAINS_H

#include <commproto/messages/SinglePropertyChain.h>
#include <commproto/messages/DoublePropertyChain.h>
#include <commproto/messages/EmptyChain.h>


namespace commproto
{
	namespace diagnostics
	{
		MAKE_EMPTY_CHAIN(RequestAllConnections);
		MAKE_DOUBLE_PROP_CHAIN(AllChannelsResponse, std::vector<std::string>, std::vector<uint32_t>);

	}
	DEFINE_DATA_TYPE(diagnostics::AllChannelsResponse);
	DEFINE_DATA_TYPE(diagnostics::RequestAllConnections);
	
}

#endif //COMMPROTO_SERVICE_CHAINS_H