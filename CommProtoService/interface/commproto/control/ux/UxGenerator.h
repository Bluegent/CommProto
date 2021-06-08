#ifndef UX_GENERATOR_I_H
#define UX_GENERATOR_I_H
#include <commproto/control/ux/Control.h>
#include <commproto/common/Common.h>

namespace commproto
{
	namespace control
	{
		namespace ux
		{
			class UxGenerator
			{
			public:
				virtual ~UxGenerator() = default;
				virtual UxContainerHandle generate(const Control & handle) = 0;
				virtual UxContainerHandle generateNotification(const Control & handle, const std::string & text, const uint32_t actionId) = 0;
				virtual void send(Message && msg) const = 0;
				virtual void notifyUpdate(const uint32_t id) const = 0;
				virtual UxContainerHandle generateText(const std::string & text) const = 0;
			};

			using GeneratorHandle = std::shared_ptr<UxGenerator>;
		}
	}
}

#endif