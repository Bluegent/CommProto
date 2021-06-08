#include "ProgressBarImpl.h"

namespace commproto
{
	namespace control
	{
		namespace ux
		{
			void ProgressBarHandler::handle(messages::MessageBase&& data)
			{
				endpoint::ProgressBarMessage& message = static_cast<endpoint::ProgressBarMessage&>(data);
				ControlHandle label = std::make_shared<ProgressBarImpl>(message.prop2, message.prop, controller->getGenerator(),message.prop3);
				controller->addControl(label);
			}

			void ProgressBarUpdateHandler::handle(messages::MessageBase&& data)
			{
				endpoint::ProgressBarUpdate& message = static_cast<endpoint::ProgressBarUpdate&>(data);
				ControlHandle bar = controller->getControl(message.prop);
				if (!bar)
				{
					return;
				}

				ProgressBarHandle cast = std::static_pointer_cast<ProgressBarImpl>(bar);
				if (!cast)
				{
					return;
				}
				cast->setProgress(message.prop2);
			}

			ProgressBarImpl::ProgressBarImpl(const std::string& name, const uint32_t id, const GeneratorHandle& generator_, const uint32_t progress_)
				: ProgressBar(name, id)
				, generator{ generator_ }
				, progress{ progress_ }
			{
			}

			void ProgressBarImpl::setProgress(const uint32_t progress_)
			{
				if (progress_ > 100)
				{
					progress = 100;
				}
				else
				{
					progress = progress_;
				}
				generator->notifyUpdate(id);
			}

			uint32_t ProgressBarImpl::getProgress() const
			{
				return progress;
			}

			UxContainerHandle ProgressBarImpl::getUx()
			{
				return generator->generate(*this);
			}
		}
	}
}
