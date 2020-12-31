
#ifndef PLUGIN_MANAGER_H
#define PLUGIN_MANAGER_H

#include "plugin.h"
#include <map>
#include <unordered_map>
#include <memory>
#include <vector>

#include "Locks.h"

class CDynamicLibrary;
struct IObjectAdapter;

class PluginManager {
    using DynamicLibraryMap = std::map<std::string, std::shared_ptr<CDynamicLibrary>>;
    using ExitFuncVec = std::vector<INU_ExitFunc>;
    using RegistrationVec = std::vector<INU_RegisterParams>;

public:
    using RegistrationMap = std::map<std::string, INU_RegisterParams>;
    using ServicesRegistrationMap = std::map<uint32_t, RegistrationMap>;

    static PluginManager& GetInstance();
    static int InitializePlugin(INU_InitFunc iInitFunc);

    int LoadAll(const std::string& iPluginDirectory, INU_InvokeServiceFunc iFunc = nullptr);
    int LoadByPath(const std::string& iPath);

    void* CreateObject(const std::string& iObjectType, unsigned int iServiceId, IObjectAdapter& iAdapter);

    int Shutdown();
    static int RegisterObject(const char* iObjectName, const INU_RegisterParams* iParams);
    const ServicesRegistrationMap& GetServicesRegistrationMap();
    const DynamicLibraryMap& GetDynamicLibraryMap();
    INU_PlatformServices& GetPlatformServices();

private:
    ~PluginManager();
    PluginManager();
    PluginManager(const PluginManager&);

    CDynamicLibrary* LoadLibrary(const std::string& iPath, std::string& oErrorString);

private:
    //static std::shared_timed_mutex mRWMutex;
    // TODO: [PLUGIN] static BLock mLocker;
    INU_PlatformServices mPlatformServices;
    DynamicLibraryMap mDynamicLibraryMap;
    std::map<unsigned long, std::string> mInvokeIds;  // hold invoke ids
    ExitFuncVec mExitFuncVec;

    //RegistrationMap mExactMatchMap; // register exact-match object types
    ServicesRegistrationMap mRegistratedObjectsMap;
    RegistrationVec mWildCardVec; // wild card ('*') object types
};

#endif
