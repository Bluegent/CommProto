#include "FileLoader.h"

#include <fstream>
#include <sstream>

std::string FileLoader::getFileContents(const std::string & path)
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
