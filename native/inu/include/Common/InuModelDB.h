
#ifndef __INUMODELDB__
#define __INUMODELDB__

#include "InuModel.h"
#include "InuSensorDBDefs.h"

#include <string>
#include <memory>

namespace InuCommon
{
    ////////////////////////////////////////////////////////////////////////
    /// \brief    Access to CInuModelDB database 
    ///
    /// Role: Represents InuModel database.
    /// InuSensors is module/device produced by Inuitive and should be access by any Host SW component 
    ///
    /// Responsibilities: 
    ///      1. Singleton
    ///      3. Access to any CInuModel that is held by this database.
    ////////////////////////////////////////////////////////////////////////
    class INUSENSORSDB_API CInuModelDB
    {
    
    public:

        virtual ~CInuModelDB() {}  

        /// \brief    Get the singleton object.
        /// \return CInuModelDB&    Reference to singleton object, this command always succeeds.
        static std::shared_ptr<CInuModelDB> GetTheInstance();
        
        // \brief    Get the sensor models DB
        /// \return CInuModelDB&    Reference to sensor models DB, this command always succeeds.
        const std::map<std::pair<std::string, std::string>, CInuModel>& GetAllModels() const { return mInuModels; };

        // \brief    Load the sensor models DB
        /// \return bool    true if loading succeeds.
        bool Load();

    private:
 
        /// \brief     Singleton instance
        static std::shared_ptr<CInuModelDB>	sTheInstance;

        /// \brief     Synchronization object for singleton creation 
        //static BLock sConstructionLocker;

        /// \brief  Collection of all used InuModels. 
        /// Some of them might be loaded from DB while the other may be loaded locally. The key is a pair of Model Name and HW Revision
        std::map<std::pair<std::string, std::string>, CInuModel>  mInuModels;

        // Object can be created only by Singleton function
        CInuModelDB() {}

        /// \brief     Create the singleton object  (it is in separate method, in order to achieved more efficient run time) 
        static void   CreateTheInstance();        
    };

    inline std::shared_ptr<CInuModelDB> InuCommon::CInuModelDB::GetTheInstance() 
    {
        //Hold a reference to the static instance.
        if (sTheInstance == nullptr) 
        {
            CreateTheInstance();
        }
        return sTheInstance;
    }


}
#endif
