#include <commproto/service/Dispatch.h>
#include <commproto/logger/Logging.h>

namespace commproto {
	namespace service {
		Dispatch::Dispatch()
			: idCounter{1}
			, checkAlive(true)
		{
		}



		void Dispatch::sendTo(const uint32_t sender, const std::string& name, const Message& msg)
		{
			std::lock_guard<std::mutex> lock(connectionMutex);
			const uint32_t id = getId(name);
			sendToNoLock(sender,id, msg);
			
		}

		void Dispatch::sendTo(const uint32_t sender, const uint32_t id, const commproto::Message& msg)
		{
			std::lock_guard<std::mutex> lock(connectionMutex);
			sendToNoLock(sender,id, msg);
		}

		void Dispatch::sendAll(const Message& msg)
		{
			for (auto it = connections.begin(); it != connections.end(); ++it)
			{
				it->second->send(msg);
			}
		}

		void Dispatch::addConnection(const sockets::SocketHandle& connection)
		{
			std::lock_guard<std::mutex> lock(connectionMutex);
			const uint32_t connectionId = idCounter++;
			ConnectionHandle newCon = std::make_shared<Connection>(connectionId, connection, this);
			connections.insert({ connectionId,  newCon });
			newCon->start();
		}

		void Dispatch::removeConnection(const uint32_t id)
		{	
			auto it = connections.find(id);

			if (it == connections.end())
			{
				return;
			}

			LOG_INFO("[Dispatch] Removing connection \"%s\"(%d)", it->second->getName().c_str(), id);
			connectionMapping.erase(it->second->getName());
			connections.erase(it);
			unsubscribeAllNoLock(id);
		}

		void Dispatch::registerChannel(const uint32_t id, const std::string& name)
		{
			auto connection = connections.find(id);
			if(connectionMapping.find(name) != connectionMapping.end() || connection == connections.end())
			{
				return;
			}

			connectionMapping.insert({ name,id });
			connection->second->setName(name);
			subscribeToNewConnection(connection->second);
		}

		void Dispatch::subsribeAll(const uint32_t id)
		{
			std::lock_guard<std::mutex> lock(connectionMutex);
			auto it = connections.find(id);

			if (it == connections.end())
			{
				return;
			}
			subscribedToAll.push_back(it->second);
			addToAllAsSubscriber(it->second);
		}

		void Dispatch::unsubsribeAll(const uint32_t id)
		{
			std::lock_guard<std::mutex> lock(connectionMutex);
			unsubscribeAllNoLock(id);
		}

		ConnectionHandle Dispatch::getConnection(const std::string& name) const
		{
			return getConnection(getId(name));
		}

		ConnectionHandle Dispatch::getConnection(const uint32_t id) const
		{
			auto it = connections.find(id);

			if (it == connections.end())
			{
				return ConnectionHandle();
			}

			return it->second;
		}

		Dispatch::~Dispatch()
		{
			checkAlive = false;
			checkAliveThread->join();
			for (auto connection : connections)
			{
				connection.second->stop();
				connection.second->clearSubscriptions();
			}
			connections.clear();
		}

		uint32_t Dispatch::getId(const std::string& name) const
		{
			auto idIt = connectionMapping.find(name);
			if (idIt == connectionMapping.end())
			{
				return 0;
			}
			return idIt->second;
		}

		void Dispatch::addToAllAsSubscriber(const ConnectionHandle& connection)
		{
			for(auto it = connections.begin();it != connections.end(); ++it)
			{
				if(it->second == connection)
				{
					continue;
				}
				it->second->registerSubscription(connection);
			}
		}

		void Dispatch::removeFromAllAsSubscriber(const ConnectionHandle& connection)
		{
			for (auto it = connections.begin(); it != connections.end(); ++it)
			{
				it->second->unregisterSubscription(connection);
			}
		}

		void Dispatch::subscribeToNewConnection(const ConnectionHandle& connection)
		{
			for(auto subscriber : subscribedToAll)
			{
				subscriber->subscribe(connection->getName());
			}
		}

		void Dispatch::unsubscribeAllNoLock(const uint32_t id)
		{
			auto connection = connections.find(id);
			if (connection == connections.end())
			{
				return;
			}
			auto it = std::find(subscribedToAll.begin(), subscribedToAll.end(), connection->second);
			if (it == subscribedToAll.end())
			{
				return;
			}
			removeFromAllAsSubscriber(*it);
			subscribedToAll.erase(it);
		}

		void Dispatch::checkActiveConnections()
		{
			std::lock_guard<std::mutex> lock(connectionMutex);
			
			std::vector<uint32_t> deadConnections;

			for(auto it = connections.begin(); it != connections.end();++it)
			{
				if(!it->second->isRunning())
				{
					deadConnections.push_back(it->first);
				}
			}

			for(auto id : deadConnections)
			{
				removeConnection(id);
			}
		}

		void Dispatch::startCheckingConnections()
		{
			checkAliveThread = std::make_unique<std::thread>([this]() {
				while (checkAlive) {
					this->checkActiveConnections();
					std::this_thread::sleep_for(std::chrono::milliseconds(100));
				}
			});
		}

		void Dispatch::sendToNoLock(const uint32_t senderId, const uint32_t id, const commproto::Message& msg)
		{
			LOG_INFO("[Dispatch] Sending message from %d to %d", senderId, id);
			auto sender = connections.find(senderId);
			auto receiver = connections.find(id);
			if (sender == connections.end() || receiver == connections.end())
			{
				return;
			}
			sender->second->handshake(receiver->second);
			receiver->second->send(msg);
		}
	}
}
