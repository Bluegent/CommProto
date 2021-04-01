#ifndef BUTTON_IMPL_H
#define BUTTON_IMPL_H
#include <functional>
#include <commproto/control/Button.h>
#include <commproto/messages/SinglePropertyChain.h>
#include <commproto/parser/Handler.h>
#include <commproto/control/UIController.h>

namespace commproto
{
	namespace control {
		namespace ux {

			MAKE_SINGLE_PROP_MESSAGE(PressButtonMessage, std::string);

			using PressButtonParser = messages::SinglePropertyParser<std::string>;
			using PressButtonSerializer = messages::SinglePropertySerializer<std::string>;

			class PressButtonHandler : public parser::Handler
			{
			public:
				PressButtonHandler(const endpoint::UIControllerHandle& controller_);
				void handle(messages::MessageBase&& data) override;
			private:
				endpoint::UIControllerHandle controller;
			};

			class Generator;
			using GeneratorHandle = std::shared_ptr<Generator>;

			class ButtonImpl : public Button {
			public:
				ButtonImpl(const std::string& name_, const uint32_t pressId, const GeneratorHandle& generator_);

				void press() override;

				std::string getUx() override;
			private:
				GeneratorHandle generator;
				const uint32_t pressId;
			};
		}

		namespace endpoint {

			MAKE_SINGLE_PROP_MESSAGE(ButtonMessage, std::string);

			using ButtonParser = messages::SinglePropertyParser<std::string>;
			using ButtonSerializer = messages::SinglePropertySerializer<std::string>;

			class ButtonHandler : public parser::Handler
			{
			public:
				ButtonHandler(const ux::UIControllerHandle& controller_);
				void handle(messages::MessageBase&& data) override;
			private:
				ux::UIControllerHandle controller;
			};


			class ButtonImpl : public Button {
			public:
				ButtonImpl(const std::string& name, const uint32_t buttonId, const ButtonAction& action_);

				void press() override;

				Message serialize() const override;
			private:
				ButtonAction action;
				const uint32_t buttonId;
			};
		}

	}

	DEFINE_DATA_TYPE(control::endpoint::ButtonMessage);
	DEFINE_DATA_TYPE(control::ux::PressButtonMessage);

}


#endif // BUTTON_IMPL_H