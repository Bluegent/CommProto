#ifndef COMMPROTO_PARSER_DELEGATOR_H
#define COMMPROTO_PARSER_DELEGATOR_H

#include <map>

#include <commproto/parser/Parser.h>
#include <commproto/parser/ByteStream.h>
#include <commproto/messages/MessageName.h>

namespace commproto
{
	namespace parser
	{

		enum class MappingResult
		{
			NoMatchingParser,
			AlreadyRegistered,
			Success

		};
		class ParserDelegator
		{
		public:
			ParserDelegator()
				:warnOnNoParser(true)
			{
				
			}
			virtual ~ParserDelegator() = default;
			template <class T>
			void registerParser(const ParserHandle & parser);

			template <class T>
			bool hasMapping() const;

			virtual MappingResult registerMapping(const std::string & name, uint32_t id);
			std::map<std::string, uint32_t> getMappings() const;

			virtual bool parse(Message & msg);
			void setNoParserWarining(const bool warning);

			

		private:

			void registerParserInternal(const std::string & typeName, const ParserHandle & parser);
			bool hasMappingInternal(const std::string & name) const;
			std::map<std::string, ParserHandle> nameToParser;
			std::map<std::string, uint32_t> nameToId;
			std::map<uint32_t, ParserHandle> idToParser;
			bool warnOnNoParser;
		};

		template <class T>
		void ParserDelegator::registerParser(const ParserHandle& parser)
		{
			registerParserInternal(MessageName<T>::name(),parser);
		}

		template <class T>
		bool ParserDelegator::hasMapping() const
		{
			return hasMappingInternal(MessageName<T>::name());
		}


		using ParserDelegatorHandle = std::shared_ptr<ParserDelegator>;
	}
}

#endif