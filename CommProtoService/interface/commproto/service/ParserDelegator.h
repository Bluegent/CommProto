#ifndef COMMPROTO_SERVICE_PARSER_DELEGATOR_H
#define COMMPROTO_SERVICE_PARSER_DELEGATOR_H

#include <map>

#include <commproto/parser/ParserDelegator.h>

namespace commproto
{
	namespace service
	{
		class Connection;

		class ParserDelegator : public parser::ParserDelegator
		{
		public:
			ParserDelegator(Connection& dispatch);
			parser::MappingResult registerMapping(const std::string & name, uint32_t id) override;
			bool parse(Message & msg) override;
		private:
			Connection& owner;
		};

		using ParserDelegatorHandle = std::shared_ptr<ParserDelegator>;
	}
}

#endif