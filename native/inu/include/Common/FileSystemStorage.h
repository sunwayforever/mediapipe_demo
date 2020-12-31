
#ifndef __FILESYSTEMSTORAGE_H__
#define __FILESYSTEMSTORAGE_H__

#include "Storage.h"
#include <fstream>

namespace InuCommon
{
    ///////////////////////////////////////////////////////////////////////
    // Class: CFileSystemStorage
    // Role: XML storage for persistent data
    // Inherits: IStorage 
    // Responsibilities: 
    //          1. Implements IStorage interface
    //          2. Knows how to read/write from XML file
    // Comments: This is a "ConcreteImplementor" of Bridge design pattern
    ///////////////////////////////////////////////////////////////////////
    class COMMONUTILITIES_API CFileSystemStorage : public IStorage
    {
        // It should be a friend of IStorage because of the private constructor 
        friend class IStorage;

    public:
        virtual ~CFileSystemStorage() {}

        // --------------------------------
        // IStorage interface implementation
        // --------------------------------
        virtual bool        SelectTable(const std::string& tableName, bool doTraverse = false) override;
        virtual bool        AddTable(const std::string& tableName) override { return false; }// TBD 

        virtual bool        Init() override;
        virtual bool        Commit() override;

        virtual bool        GetNextRecord(std::string& key, std::string& record) override;

        // Implementation of virtual GetNextRecord. It reads the next record from file stream 
        static bool         GetNextRecord(std::string& key, std::string& value, std::istream& inputFile);

        const std::string&  GetStorageDir() const override { return mStorageDir; }
        const std::string&  GetStorageName() const { return mStorageName; }

    protected:

        // Overwrite base class's abstract methods
        virtual bool        GetString(std::string& result, const std::string& key) override; 
        virtual bool        SetString(const std::string& key, const std::string& value) override; 

    private:

#ifdef _MSC_VER
#pragma warning(disable : 4251)
#endif

        std::stringstream mCurrentFileContents;

        std::string     mStorageName;
        std::string     mStorageDir;
        std::string     mTableName;

#ifdef _MSC_VER
#pragma warning(default : 4251)
#endif

        // Can be constructed only by IStorage
        CFileSystemStorage(const std::string& iStorageDir,
                           const std::string& iStorageName) : mStorageName(iStorageName), mStorageDir(iStorageDir)  {} 

    };

}


#endif
