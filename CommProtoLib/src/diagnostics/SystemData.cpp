#include <commproto/diagnostics/SystemData.h>
#include <SystemDataImpl.h>

namespace commproto
{
	namespace diagnostics
	{
		SystemDataHandle SystemData::build()
		{
			SystemDataHandle result = std::make_shared<SystemDataImpl>();
			return result;
		}
	}
}
