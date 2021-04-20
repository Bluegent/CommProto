#ifndef UX_FILE_LOADER_H
#define UX_FILE_LOADER_H

#include <string>

class FileLoader {
public:
	static std::string getFileContents(const std::string & path);
};

#endif//FILE_LOADER_H
