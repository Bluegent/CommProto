#include <commproto/parser/ParserDelegatorUtils.h>
#include <commproto/parser/MappingType.h>
#include <commproto/messages/KeepAlive.h>
#include <commproto/logger/Logging.h>

namespace commproto
{
	namespace parser
	{

		class MappingTypeParser : public Parser
		{
		public:
			MappingTypeParser(const HandlerHandle & handler) : Parser{ handler } {}
			void parse(ByteStream&& reader) override
			{
				std::string name;
				uint32_t typeId;
				uint32_t sender = 0;
				reader.read(sender);
				reader.read(name);
				reader.read(typeId);
				handler->handle(std::move(messages::MappingType(name, typeId)));
			}
		};


		class MappingTypeHandler : public Handler
		{
		public:
			MappingTypeHandler(const ParserDelegatorHandle & delegator_)
				: delegator{ delegator_ }
			{

			}

			void handle(messages::MessageBase && data) override
			{
				messages::MappingType & mapping = static_cast<messages::MappingType&>(data);
				delegator->registerMapping(mapping.name, mapping.typeId);
			}

		private:
			ParserDelegatorHandle delegator;
		};


		void DelegatorUtils::buildBase(const ParserDelegatorHandle & delegator)
		{

			ParserHandle mappingParser = std::make_shared<MappingTypeParser>(std::make_shared<MappingTypeHandler>(delegator));

			delegator->registerParser<messages::MappingType>(mappingParser);
			delegator->registerMapping(MessageName<messages::MappingType>::name(), 0);

			ParserHandle keepAliveParser = std::make_shared<messages::KeepAliveParser>();
			delegator->registerParser<messages::KeepAliveMessage>(keepAliveParser);
		}

		void DelegatorUtils::sendMappings(const ParserDelegatorHandle& delegator, const sockets::SocketHandle& socket)
		{
			auto mappings = delegator->getMappings();
			for (auto it = mappings.begin(); it != mappings.end(); ++it)
			{
				socket->sendBytes(messages::MappingTypeSerializer::serialize(std::move(messages::MappingType(it->first, it->second))));
			}
		}
	}
}
