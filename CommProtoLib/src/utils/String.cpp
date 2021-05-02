#include <commproto/utils/String.h>
#include <sstream>

namespace commproto
{
	namespace utils {

		void replaceAll(std::string& target, const std::string& substr, const std::string& replace)
		{
			std::string::size_type it = target.find(substr);
			while (it != std::string::npos)
			{
				target.replace(it, substr.size(), replace);
				it = target.find(substr, it + replace.size());
			}
		}

		std::string getString(const uint32_t number)
		{
			std::stringstream stream;
			stream << number;
			return stream.str();
		}

		std::string getString(const float value, const uint32_t precision)
		{
			std::stringstream stream;
			stream.precision(precision);
			stream << value;
			return stream.str();
		}
	}
}
