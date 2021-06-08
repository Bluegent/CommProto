#ifndef UX_NOTIFICATION_IMPL_H
#define UX_NOTIFICATION_IMPL_H
#include <commproto/control/ux/Notification.h>
#include <commproto/control/ux/UIController.h>
#include <commproto/control/NotificationChains.h>
#include <commproto/control/ux/UxGenerator.h>
#include "BaseControlType.h"

namespace commproto
{
	namespace control
	{
		namespace ux
		{

			class NotificationHandler : public parser::Handler
			{
			public:
				NotificationHandler(const UIControllerHandle & controller_) : controller{ controller_ } {}
				void handle(messages::MessageBase&& data) override;
			private:
				UIControllerHandle controller;
			};

			class DisplayNotificationHandler : public  parser::Handler
			{
			public:
				DisplayNotificationHandler(const UIControllerHandle & controller_) : controller{ controller_ } {}
				void handle(messages::MessageBase&& data) override;
			private:
				UIControllerHandle controller;
			};

			class NotificationImpl : public Notification
			{
			public:
				NotificationImpl(const std::string& name, uint32_t id, const uint32_t executeId, const std::vector<std::string> & options, const GeneratorHandle& generator_)
					: Notification(name, id)
					, executeId{ executeId }
					, options{ options }
					, generator{ generator_ }
				{
				}

				UxContainerHandle getUx() override;
				void execute(const std::string& option,const uint32_t actionId) override;
				std::vector<std::string> getOptions() const override;
				UxContainerHandle getUx(const std::string& text, const uint32_t action) const override;
				uint32_t getType() const override { return  static_cast<uint32_t>(BaseControlType::Notification); }
			private:
				const uint32_t executeId;
				const std::vector<std::string> options;
				GeneratorHandle generator;
			};
		}
	}
}

#endif //UX_NOTIFICATION_IMPL_H