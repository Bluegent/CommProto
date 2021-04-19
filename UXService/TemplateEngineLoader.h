#ifndef TEMPLATE_ENGINE_LOADER_H
#define TEMPLATE_ENGINE_LOADER_H
#include <commproto/control/ux/TemplateEngine.h>

class TemplateEngineLoader
{
public:
	TemplateEngineLoader(const commproto::control::ux::TemplateEngineHandle & engine_);
	void load(std::string & path) const;
private:
	commproto::control::ux::TemplateEngineHandle engine;
};



#endif// TEMPLATE_ENGINE_LOADER_H
