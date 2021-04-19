#ifndef UX_DELEGATOR_PROVIDER
#define UX_DELEGATOR_PROVIDER

#include <commproto/parser/ParserDelegator.h>
#include <commproto/control/ux/UxControllers.h>
#include <commproto/control/ux/UIFactory.h>
#include <commproto/endpoint/DelegatorProvider.h>



class UXServiceProvider : public  commproto::endpoint::DelegatorProvider {
public:
	UXServiceProvider(const commproto::messages::TypeMapperHandle& mapper_, const commproto::sockets::SocketHandle& socket_, const commproto::control::ux::UxControllersHandle& controllers, const commproto::control::ux::TemplateEngineHandle & engine);
	commproto::parser::ParserDelegatorHandle provide(const std::string& name, const uint32_t id) override;

private:
	commproto::messages::TypeMapperHandle mapper;
	commproto::sockets::SocketHandle socket;
	commproto::control::ux::UxControllersHandle controllers; 
	const commproto::control::ux::TemplateEngineHandle engine;
};

#endif // UX_DELEGATOR_PROVIDER