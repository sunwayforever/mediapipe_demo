
#ifndef INU_PLUGIN_H
#define INU_PLUGIN_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum INU_ProgrammingLanguage {
    INU_ProgrammingLanguage_C,
    INU_ProgrammingLanguage_CPP
} INU_ProgrammingLanguage;

struct INU_PlatformServices;

typedef struct INU_ObjectParams {
    const char* objectType;
    const struct INU_PlatformServices* platformServices;
} INU_ObjectParams;

typedef struct INU_PluginAPI_Version {
    int major;
    int minor;
} INU_PluginAPI_Version;

typedef void* (*INU_CreateFunc)(INU_ObjectParams*);
typedef int (*INU_DestroyFunc)(void*);

typedef struct INU_RegisterParams {
    INU_PluginAPI_Version version;
    INU_CreateFunc createFunc;
    INU_DestroyFunc destroyFunc;
    unsigned int serviceTypeID;
    INU_ProgrammingLanguage programmingLanguage;
} INU_RegisterParams;

typedef int (*INU_RegisterFunc)(const char* nodeType, const INU_RegisterParams* params);
typedef int (*INU_InvokeServiceFunc)(unsigned long callerId, const char* serviceName, void* serviceParams);

typedef struct INU_PlatformServices {
    INU_PluginAPI_Version version;

    INU_RegisterFunc registerObject;
    INU_InvokeServiceFunc invokeService;
    unsigned long invokeId;
} INU_PlatformServices;

typedef int (*INU_ExitFunc)();

/** Type definition of the INU_initPlugin function bellow (used by PluginManager to initialize plugins)
 * Note the return type is the INU_ExitFunc (used by PluginManager to tell plugins to cleanup). If 
 * the initialization failed for any reason the plugin may report the error via the error reporting
 * function of the provided platform services. Nevertheless, it must return NULL exit func in this case
 * to let the plugin manger that the plugin wasn't initialized properly. The plugin may use the runtime
 * services - allocate memory, log messages and of course register node types.
 *
 * @param  [const INU_PlatformServices *] params - the platform services struct 
 * @retval [INU_ExitFunc] the exit func of the plugin or NULL if initialization failed.
 */
typedef INU_ExitFunc (*INU_InitFunc)(const INU_PlatformServices*);

/** 
 * Named exported entry point into the plugin
 * This definition is required even though the function 
 * is loaded from a dynamic library by name
 * and casted to INU_InitFunc. If this declaration is 
 * commented out DynamicLibrary::getSymbol() fails
 *
 * The plugin's initialization function MUST be called "INU_initPlugin"
 * (and conform to the signature of course).
 *
 * @param  [const INU_PlatformServices *] params - the platform services struct 
 * @retval [INU_ExitFunc] the exit func of the plugin or NULL if initialization failed.
 */

#ifndef PLUGIN_API
#ifdef WIN32
#define PLUGIN_API __declspec(dllimport)
#else
#define PLUGIN_API
#endif
#endif

extern
#ifdef __cplusplus
    "C"
#endif
    PLUGIN_API INU_ExitFunc
    INU_initPlugin(const INU_PlatformServices* params);

#ifdef __cplusplus
}
#endif

#endif /* INU_PLUGIN_H */
