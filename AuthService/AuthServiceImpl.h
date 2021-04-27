#ifndef AUTH_SERVICE_IMPL_H
#define AUTH_SERVICE_IMPL_H

#include <memory>
#include <commproto/stream/Stream.h>
#include <commproto/messages/TypeMapper.h>
#include <commproto/parser/ParserDelegator.h>
#include <commproto/parser/MessageBuilder.h>

#include "AuthService.h"
#include "AuthServiceUI.h"
#include <commproto/sockets/Socket.h>
#include <commproto/endpoint/ChannelParserDelegator.h>


class IdProvider
{
public:

	IdProvider(const commproto::messages::TypeMapperHandle& mapper);
	const uint32_t scanId;
	const uint32_t authorizeId;
	const uint32_t rejectId;
	const uint32_t keepAliveId;
};
using Provider = std::shared_ptr<IdProvider>;

class AuthServiceImpl : public AuthService , public std::enable_shared_from_this<AuthServiceImpl>
{
public:
	AuthServiceImpl(const commproto::stream::StreamHandle & stream_, const commproto::sockets::SocketHandle & socket);
	void scan() override;
	void setScanFinished() override;
	void handleRequest(const APData& data) override;
	void accept(const std::string& name) override;
	void reject(const std::string& name) override;
	void sendPong() override;
	void scanStarted(const uint32_t scanAmount) override;
	void scanProgress(const uint32_t complete) override;
	bool isScanning() override { return scanning; }
	void initializeDevice() override;
	void loopBlocking() override;
	void initializeDispatch() override;

private:
	commproto::stream::StreamHandle stream;
	commproto::sockets::SocketHandle socket;
	commproto::messages::TypeMapperHandle deviceMapper;
	commproto::parser::MessageBuilderHandle deviceBuilder;
	commproto::parser::ParserDelegatorHandle deviceDelegator;

	commproto::messages::TypeMapperHandle dispatchMapper;
	commproto::parser::MessageBuilderHandle dispatchBuilder;
	commproto::endpoint::ChannelParserDelegatorHandle dispatchDelegator;
	bool scanning;
	UIHandle ui;
	Provider provider;
};

#endif //AUTH_SERVICE_IMPL_H