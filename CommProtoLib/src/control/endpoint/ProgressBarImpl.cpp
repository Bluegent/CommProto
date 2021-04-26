#include "ProgressBarImpl.h"

namespace commproto
{
	namespace control
	{
		namespace endpoint
		{
			ProgressBarImpl::ProgressBarImpl(const std::string& name, const uint32_t id, const UIControllerHandle & controller_, const uint32_t barId_, const uint32_t barUpdateId_, const uint32_t progress_)
				: ProgressBar(name, id)
				, controller{ controller_ }
				, progress{ progress_ }
				, barId{ barId_ }
				, barUpdateId{ barUpdateId_ }
			{
			}

			Message ProgressBarImpl::serialize() const
			{
				return  ProgressBarMessageSerializer::serialize(ProgressBarMessage(barId, id, name, progress));
			}

			void ProgressBarImpl::setProgress(const uint32_t progress_)
			{
				if(progress_ > 100)
				{
					progress = 100;
				}
				else {
					progress = progress_;
				}

				if (!controller)
				{
					return;
				}
				controller->send(ProgressBarUpdateSerializer::serialize(ProgressBarUpdate(barUpdateId, id, progress)));
			}

			uint32_t ProgressBarImpl::getProgress() const
			{
				return progress;
			}
		}
	}
}
