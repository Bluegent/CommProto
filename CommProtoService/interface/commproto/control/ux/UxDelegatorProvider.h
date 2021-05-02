#ifndef UX_DELEGATOR_PROVIDER
#define UX_DELEGATOR_PROVIDER

#include <commproto/parser/ParserDelegator.h>
#include <commproto/control/ux/UxControllers.h>
#include <commproto/control/ux/UIFactory.h>
#include <commproto/endpoint/DelegatorProvider.h>
#include <commproto/control/ux/UxExtensionParserProvider.h>


namespace commproto
{
	namespace control
	{
		namespace ux 
		{

			class UXServiceProvider : public  endpoint::DelegatorProvider {
			public:
				UXServiceProvider(const messages::TypeMapperHandle& mapper_, const sockets::SocketHandle& socket_, const UxControllersHandle& controllers, const TemplateEngineHandle & engine);
				parser::ParserDelegatorHandle provide(const std::string& name, const uint32_t id) override;
				void addExtension(const ExtensionHandle & extension);

			private:
				messages::TypeMapperHandle mapper;
				sockets::SocketHandle socket;
				UxControllersHandle controllers;
				TemplateEngineHandle engine;
				std::vector<ExtensionHandle> extensions;
			};
		}
	}
}

#endif // UX_DELEGATOR_PROVIDER