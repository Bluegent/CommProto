#include <commproto/service/ParserDelegatorFactory.h>
#include <commproto/service/ParserDelegator.h>
#include <commproto/service/ServiceChains.h>
#include <commproto/service/SubscribeHandler.h>
#include <commproto/service/UnsubscribeHandler.h>
#include <commproto/service/RegisterChannelHandler.h>
#include <commproto/parser/ParserDelegatorUtils.h>
#include <commproto/service/SendToHandler.h>
#include "RequestAllChannelsHandler.h"
#include <commproto/service/DiagnosticChains.h>

namespace commproto {

	namespace service {

		parser::ParserDelegatorHandle ParserDelegatorFactory::build(Connection& connection, const ChannelManagerHandle & dispatch)
		{
			std::shared_ptr<parser::ParserDelegator> delegator = std::make_shared<ParserDelegator>(connection);
			parser::DelegatorUtils::buildBase(delegator);

			parser::DelegatorUtils::addParserHandlerPair<RegisterChannelParser, RegisterChannelMessage>(delegator, std::make_shared<RegisterChannelHandler>(dispatch,connection.getId()));
			parser::DelegatorUtils::addParserHandlerPair<SubscribeParser, SubscribeMessage>(delegator, std::make_shared<SubscribeHandler>(connection));
			parser::DelegatorUtils::addParserHandlerPair<UnsubscribeParser, UnsubscribeMessage>(delegator, std::make_shared<UnsubscribeHandler>(connection));
			parser::DelegatorUtils::addParserHandlerPair<SendtoParser, SendToMessage>(delegator, std::make_shared<SendToHandler>(dispatch));
			parser::DelegatorUtils::addParserHandlerPair<diagnostics::RequestAllConnectionsParser, diagnostics::RequestAllConnections>(delegator, std::make_shared<RequestAllChannelsHandler>(dispatch));

			return delegator;
		}
	}

}
