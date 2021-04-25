#ifndef UX_FILE_LOADER_H
#define UX_FILE_LOADER_H

#include <string>
#include <vector>

class FileLoader {
public:
	static std::string getFileContents(const std::string & path);
	static std::vector<std::string> getLines(const std::string & path);
};

#endif//FILE_LOADER_H
