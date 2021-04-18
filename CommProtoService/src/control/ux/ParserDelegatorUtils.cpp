#include <commproto/control/ux/ParserDelegatorUtils.h>
#include <commproto/parser/ParserDelegatorUtils.h>
#include "ButtonImpl.h"
#include "ToggleImpl.h"
#include "LabelImpl.h"
#include "NotificationImpl.h"
#include "UIControllerImpl.h"
#include "SliderImpl.h"

namespace commproto
{
	namespace control
	{
		namespace ux
		{
			void addParsers(const parser::ParserDelegatorHandle& delegator, const UIControllerHandle& controller)
			{
				parser::addParserHandlerPair<endpoint::ButtonParser, endpoint::ButtonMessage>(delegator, std::make_shared<ButtonHandler>(controller));
				parser::addParserHandlerPair<endpoint::ToggleParser, endpoint::ToggleMessage>(delegator, std::make_shared<ToggleHandler>(controller));
				parser::addParserHandlerPair<endpoint::LabelParser, endpoint::LabelMessage>(delegator, std::make_shared<LabelHandler>(controller));
				parser::addParserHandlerPair<endpoint::LabelUpdateParser, endpoint::LabelUpdateMessage>(delegator, std::make_shared<LabelUpdateHandler>(controller));
				parser::addParserHandlerPair<endpoint::SliderMessageParser, endpoint::SliderMessage>(delegator, std::make_shared<SliderHandler>(controller));
				parser::addParserHandlerPair<endpoint::NotificationParser, endpoint::NotificationMessage>(delegator, std::make_shared<NotificationHandler>(controller));
				parser::addParserHandlerPair<endpoint::DisplayNotificationParser, endpoint::DisplayNotificationMessage>(delegator, std::make_shared<DisplayNotificationHandler>(controller));
				parser::addParserHandlerPair<endpoint::ToggleControlEnabledStateParser, endpoint::ToggleControlEnabledState>(delegator, std::make_shared<ControlStateHandler>(controller));
				parser::addParserHandlerPair<endpoint::ToggleControlShownStateParser, endpoint::ToggleControlShownState>(delegator, std::make_shared<ControlShownHandler>(controller));
			}
		}
	}
}
