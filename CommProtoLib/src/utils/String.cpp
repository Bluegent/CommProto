#include <commproto/utils/String.h>

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
	}
}
