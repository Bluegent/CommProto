#ifndef PLUGIN_DEFINITIONS_H
#define PLUGIN_DEFINITIONS_H


#if WIN32
#define DLL_EXPORT_SYMBOL __declspec(dllexport)
#define DLL_IMPORT_SYMBOL __declspec(dllimport)
#else
#define DLL_EXPORT_SYMBOL __attribute__((visibility("default")))
#define DLL_IMPORT_SYMBOL
#endif

#define PLUGIN_CHAIN_SYMBOL	extendDelegatorProvider
#define PLUGIN_LOGGER_SYMBOL setLoggable




#endif //PLUGIN_DEFINITIONS_H