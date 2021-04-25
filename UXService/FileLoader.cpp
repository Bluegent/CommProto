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

std::vector<std::string> FileLoader::getLines(const std::string& path)
{
	std::vector<std::string> lines;
	std::ifstream file(path);
	if (file.is_open())
	{
		while (!file.eof())
		{
			std::string line;
			std::getline(file, line);
			lines.push_back(line);
		}
	}
	file.close();
	return lines;
}
