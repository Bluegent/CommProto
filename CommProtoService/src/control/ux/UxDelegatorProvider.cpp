#include <commproto/control/ux/UxDelegatorProvider.h>
#include <commproto/parser/ParserDelegatorUtils.h>
#include <commproto/messages/TypeMapper.h>
#include <commproto/control/ux/ParserDelegatorUtils.h>
#include <commproto/logger/Logging.h>


namespace commproto
{
	namespace control
	{
		namespace ux
		{

			parser::ParserDelegatorHandle buildSelfDelegator()
			{
				std::shared_ptr<parser::ParserDelegator> delegator = std::make_shared<parser::ParserDelegator>();
				parser::DelegatorUtils::buildBase(delegator);
				return delegator;
			}

			UXServiceProvider::UXServiceProvider(const messages::TypeMapperHandle& mapper_, const sockets::SocketHandle& socket_, const UxControllersHandle& controllers, const TemplateEngineHandle & engine_)
				: mapper{ mapper_ }
				, socket{ socket_ }
				, controllers{ controllers }
				, engine{ engine_ }
			{
			}

			parser::ParserDelegatorHandle UXServiceProvider::provide(const std::string& name, const uint32_t id)
			{
				parser::ParserDelegatorHandle delegator = buildSelfDelegator();
				auto controller = std::make_shared<UIFactory>(name, name, mapper, socket, id, engine)->build();
				addParsers(delegator, controller);

				for(const auto & ext : extensions)
				{
					ext->extend(delegator, controller);
				}

				controllers->addController(name, controller);
				controller->requestState();
				controller->startCheckingTrackers();

				LOG_INFO("Added controller for connection \"%s\" - %d", name.c_str(), id);

				return delegator;
			}

			void UXServiceProvider::addExtension(const ExtensionHandle& extension)
			{
				extensions.emplace_back(extension);
			}
		}
	}
}