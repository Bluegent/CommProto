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

uint32_t InputHelper::getOverallHealth() const
{
	float health = 0.0f;
	health += helper->data.soil.coeff * soilTracker->getTracker().getScore();
	health += helper->data.uv.coeff * uvTracker->getTracker().getScore();
	health += helper->data.humidity.coeff * humidity->getTracker().getScore();
	health += helper->data.temperature.coeff * temperature->getTracker().getScore();

	return static_cast<uint32_t>(health);
	
}

PlantHealthProvider::PlantHealthProvider(const messages::TypeMapperHandle & mapper_, const control::endpoint::UIControllerHandle & controller_, const InputHelperHandle & helper_, const OutputHelperHandle & outputHelper)
	: mapper{ mapper_ }
	, controller{ controller_ }
	, helper(helper_)
	, outHelper(outputHelper)
{

}

parser::ParserDelegatorHandle PlantHealthProvider::provide(const std::string& name, const uint32_t id)
{
	parser::ParserDelegatorHandle delegator = buildSelfDelegator();
	control::endpoint::DelegatorUtils::addParsers(delegator, controller);
	parser::DelegatorUtils::addParserHandlerPair<plant::SoilParser, plant::Soil>(delegator, std::make_shared<SoilHandler>(helper->soilTracker));
	parser::DelegatorUtils::addParserHandlerPair<plant::UvLightParser, plant::UvLight>(delegator, std::make_shared<UvHandler>(helper->uvTracker));
	parser::DelegatorUtils::addParserHandlerPair<plant::TempHumParser, plant::TempHum>(delegator, std::make_shared<DHTHandler>(helper));

	outHelper->notifyMapping(name, id);

	return delegator;
}