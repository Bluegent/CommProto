#ifndef PLUGIN_DEFINITIONS_H
#define PLUGIN_DEFINITIONS_H


#if WIN32
#define DLL_EXPORT_SYMBOL __declspec(dllexport)
#define DLL_IMPORT_SYMBOL __declspec(dllimport)
#define CALL_CONV __cdecl 
#else
#define DLL_EXPORT_SYMBOL __attribute__((visibility("default")))
#define DLL_IMPORT_SYMBOL
#define CALL_CONV
#endif

#define FUNC_NAME(FUNC) #FUNC
#define STR(thing) FUNC_NAME(thing)

#define PLUGIN_EXTEND_SYMBOL extendDelegatorProvider
#define PLUGIN_EXTEND_NAME  STR(PLUGIN_EXTEND_SYMBOL)

#define PLUGIN_STATICS_SYMBOL setStatics
#define PLUGIN_STATICS_NAME  STR(PLUGIN_STATICS_SYMBOL)






#endif //PLUGIN_DEFINITIONS_H