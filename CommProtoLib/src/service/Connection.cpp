#include <commproto/service/Dispatch.h>
#include <commproto/logger/Logging.h>
#include <commproto/service/ParserDelegatorFactory.h>
#include <commproto/endpoint/EndpointChains.h>
#include "../parser/TypeMapperImpl.h"
#include "commproto/messages/KeepAlive.h"

namespace commproto {
	namespace service {

		Connection::Connection(uint32_t id_, const commproto::sockets::SocketHandle& socket_, const DispatchHandle& dispatch_, uint32_t sleepTime_)
			: socket{ socket_ }
			, id{ id_ }
			, running{ false }
			, sleepMicro{ sleepTime_ }
			, dispatch{ dispatch_ }
			, delegator{ ParserDelegatorFactory::build(*this, dispatch_) }
			, builder{ std::make_shared<parser::MessageBuilder>(socket_,delegator)}
			, mapper{std::make_shared<messages::TypeMapperImpl>(std::make_shared<messages::TypeMapperObserver>(socket))}
			, channelMappingId(0)
			, terminationId(0)
		{
			socket->sendByte(sizeof(void*));
			channelMappingId = mapper->registerType<endpoint::ChannelMappingMessage>();
			terminationId = mapper->registerType<endpoint::ChannelTerminationMessage>();

			endpoint::RegisterIdMessage registerId(mapper->registerType<endpoint::RegisterIdMessage>(), id);
            int res = socket->sendBytes(endpoint::RegisterIdSerializer::serialize(std::move(registerId)));
            LOG_INFO("Result of sending mapping message: %d",res);
		}

		Connection::~Connection()
		{
			stop(); 
			worker->join();
			clearSubscriptions();		
		}

		void Connection::start()
		{
			if (running)
			{
				return;
			}
			running = true;
            worker = std::make_shared<std::thread>(&Connection::loop, this);
		}

		void Connection::stop()
		{
			if (!running)
			{
				return;
			}
			running = false;	
		}

		void Connection::send(const Message& msg)
		{
			messagesOut.enqueue(msg);
		}

		void Connection::receive(const Message& msg)
		{
			//TODO: find if this needs optimization
			std::lock_guard<std::mutex> lock(subscriberMutex);
			for (ConnectionHandle con : subs)
			{
				con->send(msg);
			}
		}

		void Connection::setName(const std::string& name_)
		{
			name = name_;
		}

		void Connection::notifyTermination(const uint32_t id_)
		{
			send(endpoint::ChannelTerminationSerializer::serialize(std::move(endpoint::ChannelTerminationMessage(terminationId,id_))));
		}

		bool Connection::isRunning() const
		{
			return running;
		}

		void Connection::subscribe(const std::string& channelName)
		{
			LOG_INFO("[%s-%d] Attempting to subscribe to \"%s\".",name.c_str(),id,channelName.c_str());

			if(channelName.compare("")==0)
			{
				dispatch->subsribeAll(id);
				return;
			}

			
			ConnectionHandle target = dispatch->getConnection(channelName);
			if (!target)
			{
				return;
			}
			endpoint::ChannelMappingMessage mapping(channelMappingId, target->name, target->getId());
			send(endpoint::ChannelMappingSerializer::serialize(std::move(mapping)));
			target->registerSubscription(shared_from_this());
		}

		void Connection::unsubscribe(const std::string& channelName)
		{
			LOG_INFO("[%s-%d] Attempting to unsubscribe from \"%s\".", name.c_str(), id, channelName.c_str());
			ConnectionHandle target = dispatch->getConnection(channelName);
			if (!target)
			{
				return;
			}

			target->unregisterSubscription(shared_from_this());
		}

		void Connection::registerSubscription(const ConnectionHandle& subscriber)
		{
			LOG_INFO("[%s-%d] Registered subscribtion from \"%s\".", name.c_str(),id, subscriber->name.c_str());
			std::lock_guard<std::mutex> lock(subscriberMutex);
			if(std::find(subs.begin(), subs.end(), subscriber)!=subs.end())
			{
				return;
			}
			subs.emplace_back(subscriber);

			LOG_INFO("[%s-%d] Forwarding mappings",name.c_str(),id);
			// forward current mappings
			sendMappings(subscriber);
		}

		void Connection::unregisterSubscription(const ConnectionHandle& subscriber)
		{
			LOG_INFO("[%s-%d] Unregistered subscribtion from \"%s\".", name.c_str(),id, subscriber->name.c_str());
			std::lock_guard<std::mutex> lock(subscriberMutex);
			auto it = std::find(subs.begin(), subs.end(), subscriber);
			if (it != subs.end()) {
				subs.erase(it);
			}
			auto it2 = std::find(sentMapping.begin(), sentMapping.end(), subscriber->getId());
			if(it2 != sentMapping.end())
			{
				sentMapping.erase(it2);
			}
		}

		void Connection::handshake(const ConnectionHandle& target)
		{
			sendMappings(target);
		}

		void Connection::clearSubscriptions()
		{
			std::lock_guard<std::mutex> lock(subscriberMutex);
			for(auto con : subs)
			{
				con->notifyTermination(id);
			}
			subs.clear();
		}

		void Connection::sendMappings(const ConnectionHandle& target)
		{
			if(std::find(sentMapping.begin(),sentMapping.end(),target->getId()) != sentMapping.end())
			{
				return;
			}

			sentMapping.push_back(target->getId());
			target->sendChannelMapping(name,id);
			auto mappings = delegator->getMappings();
			for (auto it = mappings.begin(); it != mappings.end(); ++it)
			{
				messages::MappingType msg = messages::MappingType(it->first, it->second);
				msg.senderId = id;
				target->send(messages::MappingTypeSerializer::serialize(std::move(msg)));
			}
		}

		void Connection::sendChannelMapping(const std::string & name_, const uint32_t id_)
		{
			endpoint::ChannelMappingMessage mapping(channelMappingId, name_, id_);
			send(endpoint::ChannelMappingSerializer::serialize(std::move(mapping)));
		}

		void Connection::loop()
		{
			LOG_INFO("[%s-%d] Starting receive loop.", name.c_str(),id);
			keepAliveMessage = messages::KeepAliveSerializer::serialize(std::move(messages::KeepAliveMessage(mapper->registerType<messages::KeepAliveMessage>())));
			static const uint32_t keepAliveTimeMs = 2000;
			auto then = std::chrono::system_clock::now().time_since_epoch();
			auto now = std::chrono::system_clock::now().time_since_epoch();
			while (running && socket)
			{
				//recv part
				builder->pollAndReadTimes(100u);
				//send part
				Message msg;
				while (messagesOut.try_dequeue(msg))
				{
					int sent = socket->sendBytes(msg);
					if (sent != msg.size())
					{
						LOG_WARNING("Connection \"%s\"[%d] interrupted.", name.c_str(),id);
						stop();
					}

				}
				//std::this_thread::sleep_for(std::chrono::microseconds(sleepMicro));
				//check if our connection is still alive
				now = std::chrono::system_clock::now().time_since_epoch();
				uint32_t diff =  std::chrono::duration_cast<std::chrono::milliseconds>( now - then).count();
				if(diff >= keepAliveTimeMs)
				{			
					then = now;
					int32_t sent = socket->sendBytes(keepAliveMessage);
					if(sent != keepAliveMessage.size())
					{
						LOG_WARNING("Connection \"%s\"[%d] interrupted ny keep alive.", name.c_str(), id);
						stop();
					}
				}
				std::this_thread::sleep_for(std::chrono::milliseconds(1));
			}
			LOG_INFO("Stopping receive loop for connection \"%s\"", name.c_str());
		}

		uint32_t Connection::getId() const
		{
			return id;
		}

		bool operator==(const Connection& lhs, const Connection& rhs)
		{
			return lhs.id == rhs.id && lhs.name.compare(rhs.name) == 0;
		}

		std::string Connection::getName() const
		{
			return name;
		}
	}
}
