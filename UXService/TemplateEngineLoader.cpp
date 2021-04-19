#include "TemplateEngineLoader.h"
#include <Poco/Path.h>
#include <Poco/DirectoryIterator.h>
#include <fstream>
#include <sstream>


TemplateEngineLoader::TemplateEngineLoader(const commproto::control::ux::TemplateEngineHandle& engine_)
	: engine{engine_}
{
}


std::string getFileContents(const std::string & path)
{
	std::ifstream file(path);
	std::stringstream stream;
	if (file.is_open())
	{
		while (!file.eof())
		{
			std::string line;
			std::getline(file, line);
			stream << line;
		}
	}
	file.close();
	return stream.str();
}

void TemplateEngineLoader::load(std::string& pathStr) const
{
	if(!engine)
	{
		return;
	}
	
	for(Poco::DirectoryIterator iter(pathStr);iter!= Poco::DirectoryIterator();++iter)
	{
		if(iter->isDirectory())
		{
			continue;
		}
		Poco::Path path = iter.path();
		if(path.getExtension() != "html")
		{
			continue;
		}
		std::string templateName = path.getFileName();
		size_t lastindex = templateName.find_last_of(".");
		if (lastindex != std::string::npos) {
			templateName = templateName.substr(0, lastindex);
		}

		std::string templateContent = getFileContents(path.absolute().toString());

		if(templateContent.empty())
		{
			continue;
		}
		engine->addTemplate(std::move(templateName), std::move(templateContent));
	}
}
