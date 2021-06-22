#include <commproto/parser/ParserDelegatorUtils.h>
#include <commproto/control/endpoint/ParserDelegatorUtils.h>
#include <plant/interface/PlantMessages.h>


#include "PlantHealthProvider.h"
#include "PlantHandlers.h"



parser::ParserDelegatorHandle buildSelfDelegator()
{
	std::shared_ptr<parser::ParserDelegator> delegator = std::make_shared<parser::ParserDelegator>();
	parser::DelegatorUtils::buildBase(delegator);
	return delegator;
}

PlantHealthProvider::PlantHealthProvider(const messages::TypeMapperHandle & mapper_, const control::endpoint::UIControllerHandle & controller_, const InputHelperHandle & helper_)
	: mapper{ mapper_ }
	, controller{ controller_ }
	, helper(helper_)
{

}

parser::ParserDelegatorHandle PlantHealthProvider::provide(const std::string& name, const uint32_t id)
{
	parser::ParserDelegatorHandle delegator = buildSelfDelegator();
	control::endpoint::DelegatorUtils::addParsers(delegator, controller);
	parser::DelegatorUtils::addParserHandlerPair<plant::SoilParser, plant::Soil>(delegator, std::make_shared<SoilHandler>(helper->soilTracker));
	parser::DelegatorUtils::addParserHandlerPair<plant::UvLightParser, plant::UvLight>(delegator, std::make_shared<UvHandler>(helper->uvTracker));

	return delegator;
}