#ifndef UX_PROGRESS_BAR_H
#define UX_PROGRESS_BAR_H
#include <commproto/control/ux/Control.h>

namespace commproto
{
	namespace control
	{
		namespace ux
		{

			class ProgressBar : public Control {
			public:
				ProgressBar(const std::string & name, const uint32_t id) : Control{ name ,id } {}
				virtual void setProgress(const uint32_t progress) = 0;
				virtual uint32_t getProgress() const = 0;
			};

			using ProgressBarHandle = std::shared_ptr<ProgressBar>;
		}
	}
}


#endif // UX_PROGRESS_BAR_H