#ifndef INU_PLUGIN_HELPER_H
#define INU_PLUGIN_HELPER_H

#include "plugin.h"

#define PLUGIN_SUCCESS                   0
#define PLUGIN_ERROR_VERSION_MISMATCH    1
#define PLUGIN_ERROR_REGISTRATION_FAILED 2

class PluginHelper {
    struct RegisterParams : public INU_RegisterParams {
        RegisterParams(INU_PluginAPI_Version v,
            INU_CreateFunc cf,
            INU_DestroyFunc df,
            uint32_t sid,
            INU_ProgrammingLanguage pl)
        {
            version = v;
            createFunc = cf;
            destroyFunc = df;
            serviceTypeID = sid;
            programmingLanguage = pl;
        }
    };

public:
    PluginHelper(const INU_PlatformServices* iParams, const INU_PluginAPI_Version iPluginVersion)
        : mParams(iParams)
        , mPluginVersion(iPluginVersion)
        , mPluginExitFunc(exitPlugin)
    {
    }

    INU_ExitFunc getPluginExitFunc()
    {
        return mPluginExitFunc;
    }

    template <typename T>
    int registerObject(const char* objectType, INU_ProgrammingLanguage pl = INU_ProgrammingLanguage_C)
    {
        int ret = PLUGIN_SUCCESS;

        // Version check
        if (mParams->version.major >= mPluginVersion.major)
        {
            RegisterParams rp(mPluginVersion, T::create, T::destroy, T::getServiceTypeID(), pl);
            int rc = mParams->registerObject(objectType, &rp);

            if (rc != PLUGIN_SUCCESS)
            {
                mPluginExitFunc = nullptr;
                ret = PLUGIN_ERROR_REGISTRATION_FAILED;
            }
        }
        else
        {
            // Version mismatch
            mPluginExitFunc = nullptr;
            ret = PLUGIN_ERROR_VERSION_MISMATCH;
        }
        return ret;
    }

    static int exitPlugin()
    {
        return 0;
    }

private:
    const INU_PlatformServices* mParams;
    const INU_PluginAPI_Version mPluginVersion;
    INU_ExitFunc mPluginExitFunc;
};

#endif // INU_PLUGIN_HELPER_H
