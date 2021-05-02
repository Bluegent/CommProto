#include <rotary/ExtensionProvider.h>
#include <commproto/parser/ParserDelegatorUtils.h>
#include <rotary/UxRotary.h>

namespace rotary
{

	void RotaryProvider::extend(const parser::ParserDelegatorHandle& delegator, const control::ux::UIControllerHandle& controller) const
	{
		auto mapper = controller->getMapper();

		parser::DelegatorUtils::addParserHandlerPair<endpoint::RotaryMessageParser,endpoint::RotaryMessage>(delegator,std::make_shared<ux::RotaryHandler>(controller));
	}
}