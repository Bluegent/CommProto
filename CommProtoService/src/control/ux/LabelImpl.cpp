#include "LabelImpl.h"

namespace commproto
{
	namespace control
	{
		
		namespace ux
		{

			LabelHandler::LabelHandler(const ux::UIControllerHandle& controller_)
				: controller{ controller_ }
			{
			}

			void LabelHandler::handle(messages::MessageBase&& data)
			{
				endpoint::LabelMessage& message = static_cast<endpoint::LabelMessage&>(data);
				ux::ControlHandle label = std::make_shared<ux::LabelImpl>(message.prop2, message.prop, message.prop3, controller->getGenerator());
				controller->addControl(label);
			}

			LabelUpdateHandler::LabelUpdateHandler(const ux::UIControllerHandle& controller_)
				: controller{ controller_ }
			{
			}

			void LabelUpdateHandler::handle(messages::MessageBase&& data)
			{
				endpoint::LabelUpdateMessage& message = static_cast<endpoint::LabelUpdateMessage&>(data);
				ux::ControlHandle label = controller->getControl(message.prop);
				if (!label)
				{
					return;
				}

				ux::LabelHandle cast = std::static_pointer_cast<ux::LabelImpl>(label);
				if (!cast)
				{
					return;
				}
				cast->setText(message.prop2);
			}

			LabelImpl::LabelImpl(const std::string& name, uint32_t id, const std::string& text_, const GeneratorHandle & generator_)
				: Label(name, id)
				, text{ text_ }
				, generator{ generator_ }
			{
			}

			UxContainerHandle LabelImpl::getUx()
			{
				return generator->generate(*this);
			}

			void LabelImpl::setText(const std::string& text_)
			{
				text = text_;
				generator->notifyUpdate(id);
			}

			std::string LabelImpl::getText() const
			{
				return text;
			}
		}
	}
}
