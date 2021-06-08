//#include "ButtonImpl.h"
#include "IdProvider.h"
#include "ButtonImpl.h"

namespace commproto
{
	namespace control
	{
		namespace ux
		{
			ButtonHandler::ButtonHandler(const ux::UIControllerHandle& controller_)
				: controller{ controller_ }
			{
			}

			void ButtonHandler::handle(messages::MessageBase&& data)
			{
				endpoint::ButtonMessage& msg = static_cast<endpoint::ButtonMessage&>(data);
				ButtonHandle button = std::make_shared<ux::ButtonImpl>(msg.prop2, msg.prop, controller->getIdProvider().buttonId, controller->getGenerator());
				controller->addControl(button);
			}

			ButtonImpl::ButtonImpl(const std::string& name_, const uint32_t id_, const uint32_t pressId_, const GeneratorHandle& generator_)
				: Button{ name_,id_ }
				, generator{ generator_ }
				, pressId{ pressId_ }
			{

			}

			void ButtonImpl::press()
			{
				Message msg = PressButtonSerializer::serialize(std::move(PressButtonMessage(pressId, id)));
				generator->send(std::move(msg));
			}

			UxContainerHandle ButtonImpl::getUx()
			{
				return generator->generate(*this);
			}

		}
	}
}
