#ifndef UX_PROGRESS_BAR_IMPL_H
#define UX_PROGRESS_BAR_IMPL_H

#include <commproto/control/ux/ProgressBar.h>
#include <commproto/control/ProgressBarChains.h>
#include <commproto/control/ux/UIController.h>

namespace commproto
{
	namespace control
	{
		namespace ux
		{


			class ProgressBarHandler : public parser::Handler
			{
			public:
				ProgressBarHandler(const UIControllerHandle& controller_) : controller{controller_}{}
				void handle(messages::MessageBase&& data) override;
			private:
				UIControllerHandle controller;
			};

			class ProgressBarUpdateHandler : public parser::Handler
			{
			public:
				ProgressBarUpdateHandler(const UIControllerHandle& controller_) : controller{ controller_ } {}
				void handle(messages::MessageBase&& data) override;
			private:
				UIControllerHandle controller;
			};

			class Generator;
			using GeneratorHandle = std::shared_ptr<Generator>;
			class ProgressBarImpl : public ProgressBar
			{
			public:
				ProgressBarImpl(const std::string& name, const uint32_t id, const GeneratorHandle& generator_, const uint32_t progress = 0 );
				void setProgress(const uint32_t progress) override;
				uint32_t getProgress() const override;
				std::string getUx() override;
			private:
				uint32_t progress;
				GeneratorHandle generator;
			};

		}
	}
}

#endif //PROGRESS_BAR_IMPL_H