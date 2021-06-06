#ifndef COMMPROTO_DISPATCH_H
#define COMMPROTO_DISPATCH_H
#include <map>
#include <thread>
#include <commproto/sockets/Socket.h>
#include <commproto/service/Connection.h>

namespace commproto {
	namespace service {

		class Dispatch : public ChannelManager, public std::enable_shared_from_this<Dispatch>
		{
		public:
			Dispatch();
			void sendTo(const uint32_t sender, const std::string& name, const commproto::Message& msg) override;
			void sendTo(const uint32_t sender, const uint32_t id, const commproto::Message& msg) override;
			void sendAll(const commproto::Message& msg) override;
			void addConnection(const commproto::sockets::SocketHandle& connection) override;
			void removeConnection(const uint32_t id) override;
			void registerChannel(const uint32_t id, const std::string & name) override;
			void subsribeAll(const uint32_t id) override;
			void unsubsribeAll(const uint32_t id) override;
			ConnectionHandle getConnection(const std::string& name) const override;
			ConnectionHandle getConnection(const uint32_t id) const override;
			void checkActiveConnections();
			void startCheckingConnections();
			std::map<std::string, uint32_t> getMapping() override;

			~Dispatch();
		private:

			uint32_t getId(const std::string & name) const;
			void addToAllAsSubscriber(const ConnectionHandle& connection);
			void removeFromAllAsSubscriber(const ConnectionHandle& connection);
			void subscribeToNewConnection(const ConnectionHandle& connection);
			void unsubscribeAllNoLock(const uint32_t id);
			void sendToNoLock(const uint32_t sender, const uint32_t id, const commproto::Message& msg);

			std::map<uint32_t, ConnectionHandle> connections;
			std::vector<ConnectionHandle> subscribedToAll;
			std::mutex connectionMutex;
			std::map<std::string, uint32_t> connectionMapping;
			uint32_t idCounter;
			std::atomic_bool checkAlive;
			std::shared_ptr<std::thread> checkAliveThread;
		};

		using DispatchHandle = std::shared_ptr<Dispatch>;

	}
}


#endif //COMMPROTO_DISPATCH_H
