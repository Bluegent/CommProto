#ifndef UX_EXTENSION_PARSER_PROVIDER_H
#define UX_EXTENSION_PARSER_PROVIDER_H
#include <commproto/control/ux/UIController.h>
#include <commproto/parser/ParserDelegator.h>

namespace commproto
{
	namespace control
	{
		namespace ux
		{
			class UxExtensionParserProvider
			{
			public:
				virtual ~UxExtensionParserProvider() = default;
				virtual void extend(const parser::ParserDelegatorHandle & delegator, const UIControllerHandle & controller)  const = 0;
			};

			using ExtensionHandle = std::shared_ptr<UxExtensionParserProvider>;
		}
	}
}


#endif //UX_EXTENSION_PARSER_PROVIDER_H