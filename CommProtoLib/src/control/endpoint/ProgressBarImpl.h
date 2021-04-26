#ifndef PROGRESS_BAR_IMPL_H
#define PROGRESS_BAR_IMPL_H

#include <commproto/control/endpoint/ProgressBar.h>
#include <commproto/control/ProgressBarChains.h>
#include <commproto/control/endpoint/UIController.h>

namespace commproto
{
	namespace control
	{
		namespace endpoint
		{
			class ProgressBarImpl : public ProgressBar
			{
			public:
				ProgressBarImpl(const std::string& name, const uint32_t id, const UIControllerHandle & controller_, const uint32_t barId_, const uint32_t barUpdateId_, const uint32_t progress_ = 0);

				Message serialize() const override;
				void setProgress(const uint32_t progress) override;

				uint32_t getProgress() const override;
			private:
				UIControllerHandle controller;
				uint32_t progress;
				const uint32_t barId;
				const uint32_t barUpdateId;
			};

		}
	}
}

#endif //PROGRESS_BAR_IMPL_H