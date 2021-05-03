#include <commproto/endpoint/ChannelParserDelegator.h>
#include <commproto/logger/Logging.h>


namespace commproto
{
	namespace endpoint
	{

		ChannelParserDelegator::ChannelParserDelegator(const DelegatorProviderHandle& provider_)
			:provider{provider_}
		{
		}

		bool ChannelParserDelegator::parse(Message& msg)
		{
			parser::ByteStream stream(msg);
			uint32_t msgId = 0;
			uint32_t senderId = 0;
			//read twice to ignore message Id
			stream.read(msgId);
			stream.read(senderId);

			auto it = delegators.find(senderId);
			if (it == delegators.end())
			{
				LOG_WARNING("Could not find a parser delegator for sender id %d", senderId);
				return false;
			}
			it->second->parse(msg);
			return true;
		}

		void ChannelParserDelegator::notifyMapping(const std::string& name, const uint32_t id)
		{
			if (delegators.find(id) != delegators.end())
			{
				return;
			}

			for(auto sub : subscribers)
			{
				sub(name, id);
			}
			channelNames.emplace(id, name);
			addDelegator(id, provider->provide(name,id));
		}

		void ChannelParserDelegator::notifyTerminationSubs(const uint32_t id)
		{
			std::string name = "";
			auto it = channelNames.find(id);
			if(it != channelNames.end())
			{
				name = it->second;
			}
			
			for (auto sub : terminationSub)
			{
				sub(name,id);
			}
		}

		void ChannelParserDelegator::addDelegator(const uint32_t id, const parser::ParserDelegatorHandle& delegator)
		{
			if (delegators.find(id) != delegators.end())
			{
				return;
			}
			delegators.insert({id,delegator});
		}

		void ChannelParserDelegator::subscribeToChannelMapping(MappingNotification& onMapping)
		{
			subscribers.push_back(onMapping);
		}

		void ChannelParserDelegator::subscribeToChannelRemoval(MappingNotification& onTermination)
		{
			terminationSub.push_back(onTermination);
		}

		void ChannelParserDelegator::notifyTermination(const uint32_t id)
		{
			auto it = delegators.find(id);
			if ( it == delegators.end())
			{
				return;
			}
			LOG_INFO("Connection %d has been terminated", it->first);
			notifyTerminationSubs(id);
			auto it2 = channelNames.find(id);
			if(it2 != channelNames.end())
			{
				channelNames.erase(it2);
			}
			delegators.erase(it);
		}
	}
}
