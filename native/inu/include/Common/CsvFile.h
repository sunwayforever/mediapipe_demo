//
// File : IregFile.h
//

#ifndef __CSVFILE_H__
#define __CSVFILE_H__

#include "CommonUtilitiesDefs.h"
#include "OSUtilities.h"

#include <vector>
#include <string>

using namespace std;

namespace InuCommon
{
    ////////////////////////////////////////////////////////////////////////
    /// \brief    Interface for csv files 
    ///
    /// Role: Utility class which provides interface for reading and writing
    ///       Inuitive's csv files format.
    ///
    /// Responsibilities: 
    ///      1. Pack and save Inuitive's csv file
    ///      2. Read data from Inuitive's csv file
    ////////////////////////////////////////////////////////////////////////
    class COMMONUTILITIES_API  CCsvFile
    {
    public:

        /// \brief    Returned error codes
        enum EError
        {
            eOK = 0,
            eFileOpen,          /// Can't open input csv file 
            eReadData,          /// Error in reading csv file
            eUnexpectedFormat,  /// Input file format is different than csv            
        };

        /// \brief    Load registers from file (config.csv). - Parse the csv file removing spaces 
        /// \param[in]   iFileName   Input file path.
        /// \param[out]  oBuffer     Output data.
        /// \param[out]  oRevision   Revision of output file.
        /// \return EError    eOK in case of success.
        static EError ReadWithoutSpaces(const std::string& iFileName, vector<vector<std::string>>& oBuffer, std::string& oRevision);

        /// \brief    Load registers from file (config.csv). 
        /// \param[in]   iFileName   Input file path.
        /// \param[out]  oBuffer     Output data.
        /// \param[out]  oRevision   Revision of output file.
        /// \return EError    eOK in case of success.
        static EError Read(const std::string& iFileName, vector<vector<std::string>>& oBuffer, std::string& oRevision);

    private:

        /// \brief		File revision 
#ifdef _MSC_VER
#pragma warning(suppress : 4251)
        static const std::string	REVISION_NAME;
#else
        static const std::string	REVISION_NAME;
#endif


        /// \brief    Load registers from file (config.csv). - Base function for all Read calls
        /// \param[in]   iTrimLine      delimiters for string parsing
        /// \param[in]   iFileName   Input file path.
        /// \param[out]  oBuffer     Output data.
        /// \param[out]  oRevision   Revision of output file.
        /// \return EError    eOK in case of success.
        static EError Read(const std::string& iFileName, vector<vector<string>>& oBuffer, const bool iTrimLine, std::string& oRevision);
    };
}


#endif
