#ifndef STRING_UTILS_H
#define STRING_UTILS_H
#include <string>


namespace commproto
{
	namespace utils
	{
		void replaceAll(std::string& target, const std::string& substr, const std::string& replace);


		std::string getString(const uint32_t number);

		std::string getString(const float value, const uint32_t precision = 3);
	}
}

#endif //STRING_UTILS_H