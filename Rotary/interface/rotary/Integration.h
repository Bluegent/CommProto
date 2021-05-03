#ifndef ROTARY_INTEGRATION_H
#define ROTARY_INTEGRATION_H

#include <commproto/plugin/Definitions.h>
#include <commproto/control/ux/UxDelegatorProvider.h>
#include <commproto/logger/Logging.h>

#ifdef __cplusplus
extern "C" {
#endif

	DLL_EXPORT_SYMBOL void CALL_CONV PLUGIN_EXTEND_SYMBOL(const commproto::control::ux::UXServiceProviderHandle & provider);

	DLL_EXPORT_SYMBOL void CALL_CONV PLUGIN_STATICS_SYMBOL(commproto::logger::Loggable  * loggable, const char* name, const uint32_t id);

#ifdef __cplusplus
}
#endif


#endif// ROTARY_INTEGRATION_H