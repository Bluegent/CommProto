#ifndef CP_DELGATOR_PROVIDER_H
#define CP_DELGATOR_PROVIDER_H
#include <commproto/parser/ParserDelegator.h>

namespace commproto
{
	namespace endpoint
	{
		class DelegatorProvider
		{
		public:
			virtual parser::ParserDelegatorHandle provide(const std::string & name, const uint32_t id) = 0;
			virtual ~DelegatorProvider() = default;
		};

		using DelegatorProviderHandle = std::shared_ptr<DelegatorProvider>;
	}
}

#endif //CP_DELGATOR_PROVIDER_H