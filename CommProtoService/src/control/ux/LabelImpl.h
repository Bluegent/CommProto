#ifndef UX_LABEL_IMPL_H
#define UX_LABEL_IMPL_H
#include <commproto/control/ux/Label.h>
#include <commproto/control/ux/UIController.h>
#include <commproto/control/LabelChains.h>
#include <commproto/control/ux/UxGenerator.h>
#include "BaseControlType.h"

namespace commproto
{
	namespace control
	{
		namespace ux
		{

			class LabelHandler : public parser::Handler
			{
			public:
				LabelHandler(const UIControllerHandle& controller_);
				void handle(messages::MessageBase&& data) override;
			private:
				UIControllerHandle controller;
			};

			class LabelUpdateHandler : public parser::Handler
			{
			public:
				LabelUpdateHandler(const UIControllerHandle& controller_);
				void handle(messages::MessageBase&& data) override;
			private:
				UIControllerHandle controller;
			};

			class LabelImpl : public Label {
			public:
				LabelImpl(const std::string& name, uint32_t id, const std::string& text_, const GeneratorHandle & generator);

				UxContainerHandle getUx() override;
				void setText(const std::string& text) override;
				std::string getText() const override;
				uint32_t getType() const override { return static_cast<uint32_t>(BaseControlType::Label); }
			private:
				std::string text;
				GeneratorHandle generator;
			};
		}
	}
}


#endif // UX_LABEL_IMPL_H