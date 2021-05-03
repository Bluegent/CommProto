#include <rotary/Integration.h>
#include <rotary/ExtensionProvider.h>
#include <commproto/messages/SenderMaping.h>


DLL_EXPORT_SYMBOL void CALL_CONV PLUGIN_EXTEND_SYMBOL(const commproto::control::ux::UXServiceProviderHandle & provider)
{
	auto extension = std::make_shared<rotary::RotaryProvider>();
	provider->addExtension(extension);
}

DLL_EXPORT_SYMBOL void CALL_CONV PLUGIN_STATICS_SYMBOL(commproto::logger::Loggable  * loggable,const char* name, const uint32_t id)
{
	commproto::logger::setLoggable(loggable);
	SenderMapping::InitializeId(id);
	SenderMapping::InitializeName(name);
}