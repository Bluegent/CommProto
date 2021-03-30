#include <commproto/service/ParserDelegator.h>
#include <commproto/logger/Logging.h>
#include <commproto/service/Connection.h>
#include "../parser/MappingType.h"

namespace commproto
{
	namespace service
	{
		ParserDelegator::ParserDelegator(Connection& dispatch_)
			:owner{dispatch_}
		{
		}

		parser::MappingResult ParserDelegator::registerMapping(const std::string& name, uint32_t id)
		{
			LOG_DEBUG("Attempting to add a new parser mapping, %s - %d", name.c_str(), id);
			parser::MappingResult registered = parser::ParserDelegator::registerMapping(name,id);
			if( id == 0)
			{
				return registered;
			}
			messages::MappingType msg = messages::MappingType(name, id);
			msg.senderId = owner.getId();
			LOG_INFO("Parser delegator forwarding mapping %s -> %d",name.c_str(), id);
			owner.receive(messages::MappingTypeSerializer::serialize(std::move(msg)));

			return registered;
		}


		Message getSize(Message & msg)
		{
			uint32_t size = msg.size();
			parser::ByteStream stream;
			stream.write(size);
			return stream.getStream();
		}

		bool ParserDelegator::parse(Message& msg)
		{
			bool success = parser::ParserDelegator::parse(msg);

			if(success)
			{
				return true;
			}
			Message size = getSize(msg);
			msg.insert(msg.begin(),size.begin(),size.end());
			
			owner.receive(msg);
			return true;
		}
	}
}
