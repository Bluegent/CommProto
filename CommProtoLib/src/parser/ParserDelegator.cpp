#include <commproto/parser/ParserDelegator.h>
#include <commproto/logger/Logging.h>

namespace commproto
{
	namespace parser
	{
		MappingResult ParserDelegator::registerMapping(const std::string& name, uint32_t id)
		{
			auto it = nameToParser.find(name);
			if (it == nameToParser.end())
			{
				nameToId.emplace(name, id);
				return MappingResult::NoMatchingParser;
			}

			if (idToParser.find(id) != idToParser.end())
			{
				return MappingResult::AlreadyRegistered;
			}
			LOG_DEBUG("Registered a new mappign type %s - %d", name.c_str(), id);
			idToParser.emplace(id, it->second);
			nameToId.emplace(name, id);
			return MappingResult::Success;
		}

		std::map<std::string, uint32_t> ParserDelegator::getMappings() const
		{
			return nameToId;
		}

		bool ParserDelegator::parse(Message& msg)
		{
			ByteStream stream(msg);
			uint32_t msgId = 0;

			stream.read(msgId);

			auto it = idToParser.find(msgId);
			if (it == idToParser.end())
			{
				if (warnOnNoParser) {
					LOG_WARNING("Could not find a parser for message id %d", msgId);
				}
				return false;
			}
			/*
			//uncomment for more parsing info
			//warning : makes it really spammy
			std::string msgName = "UNKNOWN";
			for(auto it2 = nameToId.begin(); it2!=nameToId.end();++it2)
			{
				if(it2->second == msgId)
				{
					msgName = it2->second;
				}
			}
			LOG_INFO("Parsing message %d-\"%s\"", msgId, msgName.c_str());
			*/
			it->second->parse(std::move(stream));
			return true;

		}

		void ParserDelegator::setNoParserWarining(const bool warning)
		{
			warnOnNoParser = warning;
		}

		void ParserDelegator::registerParserInternal(const std::string& typeName, const ParserHandle& parser)
		{
			auto it = nameToParser.find(typeName);

			if (it != nameToParser.end())
			{
				return;
			}
			nameToParser.emplace(typeName, parser);
		}

		bool ParserDelegator::hasMappingInternal(const std::string& name) const
		{
			return nameToId.find(name) != nameToId.end();
		}
	}
}
