
#ifndef __STORAGE_H__
#define __STORAGE_H__

#include "CommonUtilitiesDefs.h"
#include "OSUtilities.h"

#include <string>
#include <sstream>
#include <memory>

#include <boost/lexical_cast.hpp>

namespace InuCommon
{
    ///////////////////////////////////////////////////////////////////////
    // Class: IStorage
    // Role: Abstraction of Storage of persistent data
    // Responsibilities: 
    //      1. Knows how to create appropriate concrete storage
    //      2. Knows to initialize storage (from persistent repository)
    //      3. Knows to commit changes that have been done
    //      4. Knows how to access one table in storage
    //      5. Knows how to read/change one value in storage
    // Comments: This is the "Implementor" of Bridge design pattern
    ///////////////////////////////////////////////////////////////////////
    class COMMONUTILITIES_API IStorage
    {
    public:

        enum EStorageType
        {
            eXML,
            eFileSystem,
            eIniFile
        };

        virtual ~IStorage() {}

        // Create concrete storage (out of storageName) 
        static std::shared_ptr<IStorage>   CreateStorage(EStorageType type, const std::string& storageDir, const std::string& storageName);

        // Initialize storage from the persistent repository
        virtual bool        Init() = 0;

        // Commit all changes that have been done into persistent repository
        virtual bool        Commit() = 0;

        // Select one table of data repository
        virtual bool        SelectTable(const std::string& tableName, bool doTraverse = false) = 0;

        // Add new table of data repository
        virtual bool        AddTable(const std::string& tableName) = 0;

        // Get next record. Return false in case of error or end of records. 
        virtual bool        GetNextRecord(std::string& key, std::string& record) = 0;

        // Get storage directory 
        virtual const std::string&  GetStorageDir() const = 0;

        // Read one value from storage
        template<class T> 
        bool GetValue(T& result, const std::string& key, const T& defaultVal = T());

        // Write one value into storage
        template<class T> 
        bool SetValue(const T& value, const std::string& key);



    protected:

        // Basic value are strings
        virtual bool        GetString(std::string& result, const std::string& key) = 0 ; 
        virtual bool        SetString(const std::string& key, const std::string& value) = 0; 
    };

    //////////////////////////////////////////////////////////////////////
    //                      INLINE FUNCTIONS                            //
    //////////////////////////////////////////////////////////////////////

    template<class T> 
    bool IStorage::GetValue(T& result, const std::string& key, const T& defaultVal)
    {
        bool ret(true);
        std::string tmp;
        if (GetString(tmp, key) == false || tmp.empty())
        {
            result = defaultVal;
            ret = false;
        }
        else
        {
            try
            {
                result = boost::lexical_cast<T>(tmp);
            }
            catch(...)
            {
                result = defaultVal;
                ret = false;
            }
        }

        return ret;
    }

    template<class T>
    bool IStorage::SetValue(const T& value, const std::string& key)
    {
        return SetString(key, boost::lexical_cast<std::string>(value));
    }

}




#endif
