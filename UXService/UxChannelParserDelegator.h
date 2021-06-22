#ifndef UX_CHANNEL_PARSER_DELEGATOR_H
#define UX_CHANNEL_PARSER_DELEGATOR_H

#include <commproto/endpoint/ChannelParserDelegator.h>
#include <commproto/parser/Handler.h>
#include "commproto/control/ux/UxExtensionParserProvider.h"
#include "commproto/parser/ParserDelegatorUtils.h"
#include "../CommProtoLib/interface/commproto/service/ServiceChains.h"
#include "commproto/control/ButtonChains.h"
#include "../CommProtoLib/src/control/endpoint/UIControllerImpl.h"


using NoUxNotification = std::function<void(const std::string&)>;

class UxChannelParserDelegator : public commproto::endpoint::ChannelParserDelegator
{
public:
	UxChannelParserDelegator(const commproto::endpoint::DelegatorProviderHandle& provider)
		: ChannelParserDelegator(provider)
	{
	}

	void subscribeToNoUx(const NoUxNotification & sub);
	void notifyNoUx(const uint32_t id);

private:
	std::vector<NoUxNotification> noUxSub;

};

using UxChannelParserDelegatorHandle = std::shared_ptr<UxChannelParserDelegator>;


class NoUxHandler : public commproto::parser::Handler
{
public:
	NoUxHandler(const UxChannelParserDelegatorHandle & delegator_);
	void handle(commproto::messages::MessageBase&& data) override;
private:
	UxChannelParserDelegatorHandle delegator;
};


class NoUxExtender : public commproto::control::ux::UxExtensionParserProvider
{
public:
	NoUxExtender(const UxChannelParserDelegatorHandle& delegator_);

	void extend(const commproto::parser::ParserDelegatorHandle& delegator, const commproto::control::ux::UIControllerHandle& controller) const override;
private:
	UxChannelParserDelegatorHandle chDelegator;
};


#endif