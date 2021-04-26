#ifndef ENDPOINT_PROGRESS_BAR_H
#define ENDPOINT_PROGRESS_BAR_H
#include <commproto/control/endpoint/Control.h>
#include <functional>

namespace commproto
{
	namespace control
	{
		namespace endpoint
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


#endif // ENDPOINT_PROGRESS_BAR_H