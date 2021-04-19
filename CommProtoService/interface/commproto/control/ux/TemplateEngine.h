#ifndef TEMPLATE_ENGINE_H
#define TEMPLATE_ENGINE_H

#include <map>
#include <string>
#include <memory>

namespace commproto
{
	namespace control
	{
		namespace ux
		{
			class TemplateEngine
			{

			public:
				bool addTemplate(std::string && name, std::string && templateText);
				std::string getTemplate(const std::string& name);
				std::string getTemplateWithReplacements(const std::string& name, std::map<std::string, std::string> && replacements);
			private:
				std::map<std::string, std::string> templates;
			};
			using TemplateEngineHandle = std::shared_ptr<TemplateEngine>;
		}
	}
}

#endif// TEMPLATE_ENGINE_H