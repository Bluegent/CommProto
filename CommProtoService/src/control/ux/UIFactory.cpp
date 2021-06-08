#include <commproto/control/ux/UIFactory.h>
#include "UIControllerImpl.h"
#include "HtmlUxGenerator.h"

namespace commproto
{
	namespace control
	{
		namespace ux
		{
			UIFactory::UIFactory(const std::string & name, const std::string & connectionName, const messages::TypeMapperHandle& mapper, const sockets::SocketHandle& socket, const uint32_t id, const TemplateEngineHandle & engine)
				: controller{ std::make_shared<UIControllerImpl>(name,connectionName, mapper,socket,id, engine) }
			{
				controller->setGenerator(std::make_shared<HtmlGenerator>(*controller.get()));
			}

			UIControllerHandle UIFactory::build() const
			{
				return  controller;
			}
		}
	}
}
