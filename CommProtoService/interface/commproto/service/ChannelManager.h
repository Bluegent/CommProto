#ifndef COMMPROTO_CHANNEL_MANAGER_H
#define COMMPROTO_CHANNEL_MANAGER_H
#include <map>
#include <commproto/sockets/Socket.h>

namespace commproto {
	namespace service {

		class Connection;
		using ConnectionHandle = std::shared_ptr<Connection>;

		class ChannelManager
		{
		public:
			virtual ~ChannelManager() = default;
			virtual void sendTo(const uint32_t sender, const std::string& name, const commproto::Message& msg) = 0;
			virtual void sendTo(const uint32_t sender, const uint32_t id, const commproto::Message& msg) = 0;
			virtual void sendAll(const commproto::Message& msg) = 0;
			virtual void addConnection(const commproto::sockets::SocketHandle& connection) = 0;
			virtual void removeConnection(const uint32_t id) = 0 ;
			virtual void registerChannel(const uint32_t id, const std::string & name) = 0;
			virtual void subsribeAll(const uint32_t id) = 0;
			virtual void unsubsribeAll(const uint32_t id) = 0;
			virtual ConnectionHandle getConnection(const std::string& name) const = 0;
			virtual ConnectionHandle getConnection(const uint32_t id) const = 0;
			virtual std::map<std::string, uint32_t> getMapping()=0;
		};

		using ChannelManagerHandle = std::shared_ptr<ChannelManager>;

	}
}


#endif //COMMPROTO_CHANNEL_MANAGER_H
