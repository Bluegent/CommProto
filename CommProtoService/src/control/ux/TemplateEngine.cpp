#include <commproto/control/ux/TemplateEngine.h>
#include <commproto/utils/String.h>

namespace commproto
{
	namespace control
	{
		namespace ux
		{
			bool TemplateEngine::addTemplate(std::string&& name, std::string&& templateText)
			{
				if(templates.find(name) != templates.end())
				{
					return false;
				}

				templates.emplace(name, templateText);
				return true;
			}

			std::string TemplateEngine::getTemplate(const std::string& name)
			{
				auto it = templates.find(name);
				if (it == templates.end())
				{
					return std::string();
				}
				return it->second;
			}

			std::string TemplateEngine::getTemplateWithReplacements(const std::string& name, std::map<std::string, std::string>&& replacements)
			{
				std::string temp = getTemplate(name);
				if(temp.empty())
				{
					return temp;
				}
				for(auto replacement : replacements)
				{
					utils::replaceAll(temp, replacement.first, replacement.second);
				}
				return temp;
			}
		}
	}
}