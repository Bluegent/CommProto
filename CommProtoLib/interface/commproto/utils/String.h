#ifndef STRING_UTILS_H
#define STRING_UTILS_H
#include <string>


namespace commproto
{
	namespace utils
	{
		void replaceAll(std::string& target, const std::string& substr, const std::string& replace);
	}
}

#endif //STRING_UTILS_H