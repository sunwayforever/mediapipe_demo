#ifndef __METADATA_H__
#define __METADATA_H__

#include "CommonUtilitiesDefs.h"
#include<string> 
#include <map>
#include <vector>

// Forward declaration
namespace boost
{ 
    namespace filesystem 
    { 
        class path; 
    } 
};

#ifdef _MSC_VER
#pragma warning(disable : 4251)
#endif

namespace InuCommon
{
    ////////////////////////////////////////////////////////////////////////
    /// \brief    Representation of Meta data
    ///
    /// Role: Utility class which provides interface for reading and writing 
    ///       MetaData file
    ///       
    ///
    /// Responsibilities: 
    ///      1. Read MetaData file.
    ///      2. Write MetaData file.
    ///      3. Read all MetaData files.
    ////////////////////////////////////////////////////////////////////////
    class COMMONUTILITIES_API CTemperatureMetaData
    {
    public:

        /// \ brief Error codes  
        enum EMetaDataError
        {
            eOK = 0,
            eFileNotExists,
            eFileParseError,
            eInvalidData,
            eFileSystemViolation,
            eDataCannotBeObtained,
            eUnexpectedException
        };

        /// \brief     Default c'tor 
        /// Creates empty CMetaData.
        CTemperatureMetaData() {};

        /// \brief      C'tor with parameters.
        /// Used to create CMetaData with given data.
        /// Enforce the fields required to CMetaData.
        CTemperatureMetaData(const std::string& iName, int iAvrageTemperatureActual, int iAvrageTemperatureRequired);

        /// \brief      Load data from file
        /// \param[in]  iFilePath full path to file
        /// \returns    eOK             - in case of success
        ///             eFileNotExists  - in case file does not exists
        ///             eFileParseError - in case file cannot be parsed
        EMetaDataError Read(const boost::filesystem::path& iDirPath);

        /// \brief      Save data to file
        /// \pre File must have ini extension in order to be found by ReadAllMetaFiles.
        /// \param[in]  iFilePath full path to file.
        /// \returns    eOK             - in case of success.
        ///             eInvalidData    - in case there is no acceptable data.
        ///             eFileParseError - in case tree cannot be save to ini file (bad path or illegal tree) .
        EMetaDataError Write(const std::string& iFilePath) const;

        /// \brief      Fields getters
        /// \param[out] oTemperature/oName reference for returning the required value
        /// \return     eOK                   - in case of success
        ///             eDataCannotBeObtained - in case data is missing \ doesn't match it's type (e.g. non-numeric value as temperature)
        EMetaDataError GetName(std::string& oName) const { return GetStringVal(eName, oName); }

        EMetaDataError GetActualTemperature(int& oTemperature) const { return GetIntVal(eActual, oTemperature); }
		
        EMetaDataError GetRequiredTemperature(int& oTemperature) const { return GetIntVal(eRequired, oTemperature); }
 
        EMetaDataError GetFolderName(std::string& oName) const { return GetStringVal(eFolderName, oName); }

        /// \brief      Get all MetaData files in directory
        /// \param[in]  iPathToDirectory path to directory
        /// \param[out] oMetaData map from actual temperature to a CMetaData instances
        /// \post Every temperature that has legit metaData ini file descending from iPathToDirectory is a key in oMetaDataMapper
        /// \post If there is more than one legit metaData file with the same actual temperature, only one will be used
        /// \return     eOK                  - in case of success
        ///             eFileSystemViolation - in case we failed to iterate files under iPathToDirectory 
        static EMetaDataError GetAllMetaFiles(const std::string& iPathToDirectory, std::map<int, CTemperatureMetaData>& oMetaData);

    private:

        /// \ brief     Enumeration of fields
        /// Adding fields without altering the c'tor will cause compilation error
        enum EDataFields
        {
            eName,
            eActual,
            eRequired,
            eFolderName,
            eNumberOfFields,
        };

        /// \brief     Mapping between fields and their values
        std::map<EDataFields, std::string> mValuesMap;

        /// \brief     Base getInt for wrapping fields getters
        EMetaDataError GetIntVal(EDataFields iField, int& oVal) const;

        /// \brief     Base getString for wrapping fields getters
        EMetaDataError GetStringVal(EDataFields iField, std::string& oVal) const;

        /// \brief     Extension of MetaDataF files
        /// Must be static because it is being used by GetAllMetaFiles
        static const std::string mMetaDataFileExtension;

        /// \brief     Mapping between fields and their names (as appears in file) (fixed for all files)
        /// Must be static to prevent the loss of assignment operator
        static const std::map<EDataFields, std::string> mNamesMap;
    };
}

#ifdef _MSC_VER
#pragma warning(default : 4251)
#endif

#endif
