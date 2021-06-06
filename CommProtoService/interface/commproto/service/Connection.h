#ifndef COMMPROTO_SERVICE_CONNECTION_H
#define COMMPROTO_SERVICE_CONNECTION_H
#include <thread>
#include <commproto/sockets/Socket.h>
#include <commproto/parser/MessageBuilder.h>
#include <atomic>
#include <concurrentqueue.h>
#include <mutex>
#include <commproto/messages/TypeMapper.h>
#include <commproto/service/ChannelManager.h>
#include "ParserDelegator.h"


namespace commproto
{
	namespace service
	{

		class Connection;
		using ConnectionHandle = std::shared_ptr<Connection>;
		using Subscribers = std::vector<ConnectionHandle>;


		class Connection : public std::enable_shared_from_this<Connection>
		{
		public:

			Connection(uint32_t id, const sockets::SocketHandle& socket_, const ChannelManagerHandle& manager_, uint32_t sleepTimeMicro_ = 10);
			~Connection();
			void start();
			void stop();
			void loop();
			void send(const Message& msg);
			void receive(const Message& msg);
			void setName(const std::string & name);
			void notifyTermination(const uint32_t id);
			bool isRunning() const;
			std::string getName() const;

			uint32_t getId() const;

			void subscribe(const std::string & channelName);
			void unsubscribe(const std::string & channelName);

			void registerSubscription(const ConnectionHandle& subscriber);
			void unregisterSubscription(const ConnectionHandle& subscriber);
		
			void handshake(const ConnectionHandle& target);

			void sendAllChannels(const std::map<std::string,uint32_t> & mapping);

			void clearSubscriptions();
			friend bool operator==(const Connection& lhs, const Connection& rhs);
		private:

			void sendMappings(const ConnectionHandle& subscriber);
			void sendChannelMapping(const std::string & name, const uint32_t id);

			sockets::SocketHandle socket;
			std::string name;
			const uint32_t id;
            std::shared_ptr<std::thread> worker;
			moodycamel::ConcurrentQueue<Message> messagesOut;
			std::atomic_bool running;
			const uint32_t sleepMicro;
			ChannelManagerHandle manager;
			parser::ParserDelegatorHandle delegator;
			parser::MessageBuilderHandle builder;
			Subscribers subs;
			std::mutex subscriberMutex;
			messages::TypeMapperHandle mapper;
			uint32_t channelMappingId;
			Message keepAliveMessage;
			uint32_t terminationId;
			uint32_t channelResponseId;
			std::vector<uint32_t> sentMapping;
		};

	}
}

#endif //COMMPROTO_SERVICE_CONNECTION_H
