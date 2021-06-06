#ifndef COMMPROTO_SERVICE_PARSER_DELEGATOR_FACTORY_H
#define COMMPROTO_SERVICE_PARSER_DELEGATOR_FACTORY_H

#include <commproto/parser/ParserDelegator.h>
#include <commproto/service/ChannelManager.h>

namespace commproto
{
	namespace service
	{
		class Connection;
		class ParserDelegatorFactory
		{
		public:
			static parser::ParserDelegatorHandle build(Connection& connection, const ChannelManagerHandle & dispatch);
		};

	}

}
#endif // COMMPROTO_PARSER_DELEGATOR_BUILDER_H