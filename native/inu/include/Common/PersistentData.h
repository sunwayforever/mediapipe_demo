#ifndef __PERSISTENTDATA_H__
#define __PERSISTENTDATA_H__

#include "CommonUtilitiesDefs.h"

#include <memory>

namespace InuCommon
{
   class IStorage;

    ///////////////////////////////////////////////////////////////////////
    // Class: IPersistentData
    // Role: Interface for Persistent data
    // Responsibilities: 
    //      1. Load/Save object data into persistent storage
    // Comments: "Abstraction" of Bridge design pattern
    ///////////////////////////////////////////////////////////////////////
    class COMMONUTILITIES_API IPersistentData
    {
    public:

        /// \brief    Load this object from input persistent storage
        /// 
        /// This method should be implemented by any derived class. It defines how 
        /// the object is deserialized from repository. 
        /// \param[in] storage    Reference to objects repository.
        /// \return bool    true in case of successful load.
        virtual bool  Load(const std::shared_ptr<InuCommon::IStorage>& storage) = 0;


        /// \brief    Save this object into input persistent storage
        /// 
        /// This method should be implemented by any derived class. It defines how 
        /// the object is serialized into repository. 
        /// \param[in] storage    Reference to objects repository.
        /// \return bool    true in case of successful load.
        virtual bool  Save(const std::shared_ptr<InuCommon::IStorage>& storage) = 0; 
    };
}

#endif

