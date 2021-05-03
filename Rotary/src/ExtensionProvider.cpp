#include <rotary/ExtensionProvider.h>
#include <commproto/parser/ParserDelegatorUtils.h>
#include <rotary/UxRotary.h>
#include <rotary/EndpointRotary.h>

namespace rotary
{

	void RotaryProvider::extend(const parser::ParserDelegatorHandle& delegator, const control::ux::UIControllerHandle& controller) const
	{
		parser::DelegatorUtils::addParserHandlerPair<endpoint::RotaryMessageParser,endpoint::RotaryMessage>(delegator,std::make_shared<ux::RotaryHandler>(controller));
	}

	void EndpointRotaryProvider::extend(const parser::ParserDelegatorHandle& delegator, const control::endpoint::UIControllerHandle& controller)
	{
		parser::DelegatorUtils::addParserHandlerPair<ux::RotaryAdjustParser, ux::RotaryAdjust>(delegator, std::make_shared<endpoint::RotaryHandler>(controller));
	}
}
