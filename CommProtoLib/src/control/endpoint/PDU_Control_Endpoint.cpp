#include <commproto/control/endpoint/ParserDelegatorUtils.h>
#include <commproto/parser/ParserDelegatorUtils.h>
#include <commproto/control/ControllerChains.h>
#include "ButtonImpl.h"
#include "ToggleImpl.h"
#include "NotificationImpl.h"
#include "UIControllerImpl.h"
#include "SliderImpl.h"

namespace commproto
{
	namespace control
	{
		namespace endpoint
		{

			void DelegatorUtils::addParsers(const parser::ParserDelegatorHandle & delegator, const UIControllerHandle & controller)
			{
				parser::DelegatorUtils::addParserHandlerPair<ux::PressButtonParser, ux::PressButtonMessage>(delegator, std::make_shared<PressButtonHandler>(controller));
				parser::DelegatorUtils::addParserHandlerPair<ux::ToggleParser, ux::ToggleMessage>(delegator, std::make_shared<ToggleHandler>(controller));
				parser::DelegatorUtils::addParserHandlerPair<ux::NotificationResponseParser, ux::NotificationResponseMessage>(delegator, std::make_shared<NotificationResponseHandler>(controller));
				parser::DelegatorUtils::addParserHandlerPair<ux::RequestControllerStateParser, ux::RequestControllerState>(delegator, std::make_shared<RequestStateHandler>(controller));
				parser::DelegatorUtils::addParserHandlerPair<ux::SliderAdjustParser, ux::SliderAdjust>(delegator, std::make_shared<SliderAdjustHandler>(controller));
			}
		}
	}
}
