#include "ToggleImpl.h"
#include "IdProvider.h"

namespace commproto
{
	namespace control
	{
	
		namespace ux
		{
			ToggleHandler::ToggleHandler(const UIControllerHandle& controller_)
				: controller{ controller_ }
			{
			}

			void ToggleHandler::handle(messages::MessageBase&& data)
			{
				endpoint::ToggleMessage& msg = static_cast<endpoint::ToggleMessage&>(data);
				ToggleHandle button = std::make_shared<ToggleImpl>(msg.prop2, msg.prop, controller->getIdProvider().toggleId, controller->getGenerator(), msg.prop3);
				controller->addControl(button);
			}

			ToggleImpl::ToggleImpl(const std::string& name, uint32_t id, const uint32_t messageId, const GeneratorHandle& generator_, const bool defaultState)
				: Toggle(name, id)
				, messageId{ messageId }
				, generator{ generator_ }
				, state{ defaultState }
			{
			}

			UxContainerHandle ToggleImpl::getUx()
			{
				return generator->generate(*this);
			}

			void ToggleImpl::toggle()
			{
				state = !state;
				ToggleMessage msg(messageId, id);
				generator->send(ToggleSerializer::serialize(std::move(msg)));
			}

			bool ToggleImpl::getState() const
			{
				return state;
			}
		}
	}
}
