#ifndef UX_UIFACTORY_H
#define UX_UIFACTORY_H
#include <commproto/control/ux/UIController.h>
#include <commproto/sockets/Socket.h>
#include <commproto/messages/TypeMapper.h>

namespace commproto
{
	namespace control
	{

		namespace ux
		{
			class UIFactory
			{
			public:
				UIFactory(const std::string & name, const std::string & connectionName, const messages::TypeMapperHandle& mapper, const sockets::SocketHandle& socket, const uint32_t id, const TemplateEngineHandle & engine);
				UIControllerHandle build() const;
			private:
				UIControllerHandle controller;
			};
		}
	}
}


#endif // UIFACTORY_H