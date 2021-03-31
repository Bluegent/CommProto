#ifndef CONTROL_PARSER_DELEGATOR_UTILS_H
#define CONTROL_PARSER_DELEGATOR_UTILS_H

#include <commproto/parser/ParserDelegator.h>
#include <commproto/control/UIController.h>

namespace commproto
{
    namespace control
    {
		namespace endpoint 
    	{
			void addParsers(const parser::ParserDelegatorHandle & delegator, const UIControllerHandle & controller);
		}
		namespace ux
		{
			void addParsers(const parser::ParserDelegatorHandle & delegator, const UIControllerHandle & controller);
		}
    }
}

#endif //CONTROL_PARSER_DELEGATOR_UTILS_H