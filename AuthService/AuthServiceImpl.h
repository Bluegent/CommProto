#ifndef AUTH_SERVICE_IMPL_H
#define AUTH_SERVICE_IMPL_H

#include <memory>
#include <commproto/stream/Stream.h>
#include <commproto/messages/TypeMapper.h>

#include "AuthService.h"


class AuthServiceImpl : public AuthService
{
public:
	AuthServiceImpl(const commproto::stream::StreamHandle stream_);
	void scan() override;
	void setScanFinished() override;
	void handleRequest(const APData& data) override;
	void accept(const std::string& name) override;
	void reject(const std::string& name) override;
	void sendPong() override;
	void scanStarted(const uint32_t scanAmount) override;
	void scanProgress(const uint32_t complete) override;

private:
	commproto::stream::StreamHandle stream;
	commproto::messages::TypeMapperHandle mapper;
	bool scanning;
	uint32_t scanId;
	uint32_t authorizeId;
	uint32_t rejectId;
	uint32_t keepAliveId;
};

#endif //AUTH_SERVICE_IMPL_H