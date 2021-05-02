#include <rotary/Integration.h>
#include <rotary/ExtensionProvider.h>


DLL_EXPORT_SYMBOL void CALL_CONV PLUGIN_EXTEND_SYMBOL(const commproto::control::ux::UXServiceProviderHandle & provider)
{
	auto extension = std::make_shared<rotary::RotaryProvider>();
	provider->addExtension(extension);
}

DLL_EXPORT_SYMBOL void CALL_CONV PLUGIN_LOGGER_SYMBOL(commproto::logger::Loggable  * loggable)
{
	commproto::logger::setLoggable(loggable);
}