#ifndef EXTENSION_PROVIDER_H
#define EXTENSION_PROVIDER_H
#include <commproto/control/ux/UxExtensionParserProvider.h>

using namespace commproto;

namespace rotary
{
	class RotaryProvider : public control::ux::UxExtensionParserProvider
	{
	public:
		void extend(const parser::ParserDelegatorHandle& delegator, const control::ux::UIControllerHandle& controller) const override;
	};


}

#endif //EXTENSION_PROVIDER_H