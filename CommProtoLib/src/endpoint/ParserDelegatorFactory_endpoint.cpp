#include <commproto/endpoint/ParserDelegatorFactory.h>
#include <commproto/endpoint/ChannelMappingHandler.h>
#include <commproto/endpoint/EndpointChains.h>
#include <commproto/endpoint/RegisterIdHandler.h>
#include <commproto/parser/ParserDelegatorUtils.h>
#include "commproto/endpoint/ChannelTerminationHandler.h"

namespace commproto {
	namespace endpoint
	{
		parser::ParserDelegatorHandle ParserDelegatorFactory::build(const ChannelParserDelegatorHandle & channelDelegator)
		{
			std::shared_ptr<parser::ParserDelegator> delegator = std::make_shared<parser::ParserDelegator>();
			parser::DelegatorUtils::buildBase(delegator);

			parser::DelegatorUtils::addParserHandlerPair<RegisterIdParser, RegisterIdMessage>(delegator, std::make_shared<RegisterIdHandler>());
			parser::DelegatorUtils::addParserHandlerPair<ChannelMappingParser, ChannelMappingMessage>(delegator, std::make_shared<ChannelMappingHandler>(channelDelegator));
			parser::DelegatorUtils::addParserHandlerPair<ChannelTerminationParser, ChannelTerminationMessage>(delegator, std::make_shared<ChannelTerminationHandler>(channelDelegator));

			return delegator;
		}
	}
}
