#ifndef PARSER_DELEGATOR_UTILS_H
#define PARSER_DELEGATOR_UTILS_H

#include <commproto/parser/ParserDelegator.h>
#include <commproto/sockets/Socket.h>

namespace commproto
{
    namespace parser
    {
		class DelegatorUtils
		{
		public:
			template <typename ParserType, typename MessageType>
			static void addParserHandlerPair(const parser::ParserDelegatorHandle & delegator, const parser::HandlerHandle &handler);
			static void buildBase(const ParserDelegatorHandle & delegator);
			static void sendMappings(const ParserDelegatorHandle & delegator, const sockets::SocketHandle & socket);
		};

        template <typename ParserType, typename MessageType>
		void DelegatorUtils::addParserHandlerPair(const parser::ParserDelegatorHandle & delegator, const parser::HandlerHandle &handler)
		{
			parser::ParserHandle parser = std::make_shared<ParserType>(handler);

			delegator->registerParser<MessageType>(parser);
		}

		
    }
}

#endif //PARSER_DELEGATOR_UTILS_H