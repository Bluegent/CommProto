#ifndef COMMPROTO_SERVICE_CHANNEL_PARSER_DELEGATOR_H
#define COMMPROTO_SERVICE_CHANNEL_PARSER_DELEGATOR_H

#include <map>

#include <commproto/parser/ParserDelegator.h>
#include <commproto/endpoint/DelegatorProvider.h>
#include <functional>

namespace commproto
{
	namespace endpoint
	{
		using MappingNotification = std::function<void(const std::string&, const uint32_t)>;

		class ChannelParserDelegator : public parser::ParserDelegator
		{
		public:
			ChannelParserDelegator(const DelegatorProviderHandle& provider);
			bool parse(Message & msg) override;
			void notifyMapping(const std::string & name, const uint32_t id);
			
			void notifyTermination(const uint32_t id);
			void addDelegator(const uint32_t id, const parser::ParserDelegatorHandle & delegator);
			void subscribeToChannelMapping(MappingNotification& onMapping);
			void subscribeToChannelRemoval(MappingNotification& onTermination );
			std::string getChannelName(const uint32_t id);

		private:
			void notifyTerminationSubs(const uint32_t id);
			std::map<uint32_t, parser::ParserDelegatorHandle> delegators;
			std::map<uint32_t, std::string> channelNames;
			DelegatorProviderHandle provider;
			std::vector<MappingNotification> subscribers;
			std::vector<MappingNotification> terminationSub;
		};

		using ChannelParserDelegatorHandle = std::shared_ptr<ChannelParserDelegator>;
	}
}

#endif //COMMPROTO_SERVICE_CHANNEL_PARSER_DELEGATOR_H