
#ifndef __XMLSTORAGE_H__
#define __XMLSTORAGE_H__

#include "Storage.h"
#include "XmlParser.h"

namespace InuCommon
{
    ///////////////////////////////////////////////////////////////////////
    // Class: CXmlStorage
    // Role: XML storage for persistent data
    // Inherits: IStorage 
    // Responsibilities: 
    //          1. Implements IStorage interface
    //          2. Knows how to read/write from XML file
    // Comments: This is a "ConcreteImplementor" of Bridge design pattern
    ///////////////////////////////////////////////////////////////////////
    class CXmlStorage : public IStorage
    {
        // It should be a friend of IStorage because of the private constructor 
        friend class IStorage;

    public:

        // --------------------------------
        // IStorage interface implementation
        // --------------------------------
        virtual bool        SelectTable(const std::string& tableName, bool doTravers = false) override;
        virtual bool        AddTable(const std::string& tableName) override;

        virtual bool        Init() override;
        virtual bool        Commit() override;

        const std::string&  GetStorageDir() const override { return mStorageDir; }


    protected:

        // Overwrite base class's abstract methods
        virtual bool        GetString(std::string& result, const std::string& key) override; 
        virtual bool        SetString(const std::string& key, const std::string& value) override; 

        virtual bool        GetNextRecord(std::string& key, std::string& value) override;

    private:
        static const std::string        XML_EXTENSION;

        // Object that knows how to read/write from XML file
        CXmlParser      mParser;

        // XML file name
        std::string     mStorageName;
        std::string     mStorageDir;

        // Can be constructed only by IStorage
        CXmlStorage(const std::string& iStorageDir,
                    const std::string& iStorageName) : mStorageName(iStorageName), mStorageDir(iStorageDir)  {} 
    };
}


#endif
