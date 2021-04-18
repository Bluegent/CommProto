#include <commproto/control/endpoint/ParserDelegatorUtils.h>
#include <commproto/parser/ParserDelegatorUtils.h>
#include <commproto/control/ControllerChains.h>
#include "ButtonImpl.h"
#include "ToggleImpl.h"
#include "NotificationImpl.h"
#include "UIControllerImpl.h"

namespace commproto
{
	namespace control
	{
		namespace endpoint
		{

			void addParsers(const parser::ParserDelegatorHandle & delegator, const UIControllerHandle & controller)
			{
				parser::addParserHandlerPair<ux::PressButtonParser, ux::PressButtonMessage>(delegator, std::make_shared<PressButtonHandler>(controller));
				parser::addParserHandlerPair<ux::ToggleParser, ux::ToggleMessage>(delegator, std::make_shared<ToggleHandler>(controller));
				parser::addParserHandlerPair<ux::NotificationResponseParser, ux::NotificationResponseMessage>(delegator, std::make_shared<NotificationResponseHandler>(controller));
				parser::addParserHandlerPair<ux::RequestControllerStateParser, ux::RequestControllerState>(delegator, std::make_shared<RequestStateHandler>(controller));
			}
		}
	}
}
