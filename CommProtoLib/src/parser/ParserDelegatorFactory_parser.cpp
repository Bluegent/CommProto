#include <commproto/parser/ParserDelegatorFactory.h>
#include <commproto/variable/Variable.h>
#include <commproto/variable/VariableParser.h>
#include <commproto/variable/VariableHandler.h>
#include <commproto/variable/VariableMappingParser.h>
#include <commproto/variable/VariableMappingHandler.h>
#include <commproto/parser/ParserDelegatorUtils.h>

namespace commproto
{
	namespace parser
	{

		ParserDelegatorHandle ParserDelegatorFactory::build(const variable::ContextHandle& context)
		{

			ParserDelegatorHandle delegator = std::make_shared<ParserDelegator>();
			DelegatorUtils::buildBase(delegator);

			ParserHandle variableParser = std::make_shared<variable::VariableParser>(std::make_shared<variable::VariableHandler>(context));

			ParserHandle variableMappingParser = std::make_shared<variable::VariableMappingParser>(std::make_shared<variable::VariableMappingHandler>(context));

			delegator->registerParser<variable::VariableMessage>(variableParser);
			delegator->registerParser<variable::VariableMappingMessage>(variableMappingParser);
			return delegator;

		}

	}


}
