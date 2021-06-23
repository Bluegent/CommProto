#ifndef OUTPUT_HELPER_H
#define OUTPUT_HELPER_H
#include <commproto/sockets/Socket.h>
#include <commproto/parser/Handler.h>




using namespace commproto;




class OutputHelper
{
public:
	OutputHelper(const sockets::SocketHandle& socket, const std::string & targetName);
	void notifyMapping(const std::string & name, const uint32_t id);
	void startUv() const;
	void stopUv() const;
	void startPump() const;
	void stopPump() const;
	void setIds(const uint32_t soil, const uint32_t uv, const uint32_t sendTo);
private:
	sockets::SocketHandle  socket;
	std::string targetName;
	uint32_t startIrrigationId;
	uint32_t startUvId;
	uint32_t sendToId;
	uint32_t targetEpId;
};

using OutputHelperHandle = std::shared_ptr<OutputHelper>;


#endif
