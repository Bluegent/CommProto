#ifndef EXTENSION_PROVIDER_H
#define EXTENSION_PROVIDER_H
#include <commproto/control/ux/UxExtensionParserProvider.h>
#include <commproto/control/endpoint/UIController.h>
#include <commproto/plugin/Definitions.h>

using namespace commproto;

namespace rotary
{
	class RotaryProvider : public control::ux::UxExtensionParserProvider
	{
	public:
		void extend(const parser::ParserDelegatorHandle& delegator, const control::ux::UIControllerHandle& controller) const override;
	};

	class DLL_EXPORT_SYMBOL EndpointRotaryProvider
	{
	public:
		static void extend(const parser::ParserDelegatorHandle& delegator, const control::endpoint::UIControllerHandle& controller);
	};


}

#endif //EXTENSION_PROVIDER_H