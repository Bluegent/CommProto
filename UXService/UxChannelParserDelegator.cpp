#include"UxChannelParserDelegator.h"

void UxChannelParserDelegator::subscribeToNoUx(const NoUxNotification& sub)
{
	noUxSub.push_back(sub);
}

void UxChannelParserDelegator::notifyNoUx(const uint32_t id)
{
	std::string name = getChannelName(id);
	if(name.empty())
	{
		return;
	}

	for(auto & not : noUxSub)
	{
		not(name);
	}
}

NoUxHandler::NoUxHandler(const UxChannelParserDelegatorHandle& delegator_)
	:delegator(delegator_)
{
}

void NoUxHandler::handle(commproto::messages::MessageBase&& data)
{
	if(!delegator)
	{
		return;
	}

	delegator->notifyNoUx(data.senderId);
}

NoUxExtender::NoUxExtender(const UxChannelParserDelegatorHandle& delegator_): chDelegator(delegator_)
{
}

void NoUxExtender::extend(const commproto::parser::ParserDelegatorHandle& delegator, const commproto::control::ux::UIControllerHandle& controller) const
{
	commproto::parser::DelegatorUtils::addParserHandlerPair<commproto::control::endpoint::NoControllerResponseParser, commproto::control::endpoint::NoControllerResponse>(delegator, std::make_shared<NoUxHandler>(chDelegator));
}
