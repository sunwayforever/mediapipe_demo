#ifndef __CALIBRATION_TABLES_H__
#define __CALIBRATION_TABLES_H__

#include "CommonUtilitiesDefs.h"
#include <vector>

#include <boost/filesystem.hpp>

#ifdef _MSC_VER
#pragma warning(disable : 4251)
#endif

namespace  InuCommon
{ 
    ////////////////////////////////////////////////////////////////////////
    /// \brief    This class response to read & write calibration data
    ///
    /// Role: 
    /// TODO: write role 
    ///
    /// Responsibilities: 
    ///      1. Read calibration data from flash
    ///      2. Build calibration data to write to flash
    ///
    /// Comments: 
    ///
    ////////////////////////////////////////////////////////////////////////
    class COMMONUTILITIES_API CCalibrationTablesUtilities
    {
    public:
        static const std::string SW_REVISION_STR;

        enum ECompressionError
        {
            eOK=0,
            eMissingCameraFiles = 1,
            eMissingCompressionApp = 2,
            eCompressedFileReading = 3,
            eCompressionError = 4,
            eCopyFolder = 5,
            eEmptyInputBuffer = 6,
            eSensorsFolderCreating = 7,
            eTempFolderCreating = 8,
            eTempFolderCleaning = 9,
            eCompressedFileEarasing = 10,
            eCompressedFileCreating = 11,
            eCompressedFileWriting = 12,
            eCompressedFileOpenning = 13,
            eUnexpectedSWRevision = 14,
            eUnrecognizedSensorModel = 15,
            eChmodError = 16,
            eFailedToCreateMetaDataFile = 17,
        };

        /// \brief    Compress input camera calibration folder into output unsigned char buffer.
        ///
        /// \param[in]  iCalibrationData    Calibration data path
        /// \param[out] oBuffer             Output calibration data.
        /// \return     ECompressionError 
        static ECompressionError Compress(const std::string& iCalibrationData, std::vector<unsigned char>& oBuffer);


        /// \brief    Compress input camera calibration folder into output unsigned char buffer.
        ///
        /// \param[in]  iCalibrationData    Calibration data path
        /// \param[out] oBuffer             Output calibration data.
        /// \return     ECompressionError 
        static ECompressionError  Decompress(const char* iBuffer, 
                                             uint32_t iBufferSize,
                                             const std::string& iSensorID = std::string(),
                                             uint64_t iLastBurnTime = std::numeric_limits<uint64_t>::max(),
                                             const std::string& iDestinationPath = std::string());
 
        /// \brief    Copy one source calibration directory to destination path.
        ///
        /// \param[in]  iSource          Source directory name.
        /// \param[in]  iDestination     Destination path.
        /// \return     bool             true if the operation succeeded.
        static bool CopyCalibrationDir(const boost::filesystem::path& iSource, const boost::filesystem::path& iDestination);

    private:


        /// \brief  Temporary file name which is used by compression/decompression 
        static const std::string COMPRESSED_FILE_NAME;

        /// \brief  Compression/decompression application name
        static const std::string COMPRESSION_APP_NAME;

        /// \brief    brief Builds the command that decompresses the zipped file 
        /// \param[in,out] exeFile    Unzip execution command.
        /// \param[in,out] compressedFileName    Zip file name.
        /// \param[in,out] decompressFolder    Target folder.
        /// \param[in,out] iCompressionFormat    Compression format ("7z" or "zip").
        /// \return void    The command that is created by this function.
        static std::string BuildDecompressCommand(boost::filesystem::path& iExeFile, boost::filesystem::path& iCompressedFileName, boost::filesystem::path& iDecompressFolder, const std::string& iCompressionFormat);
    };
}

#ifdef _MSC_VER
#pragma warning(default : 4251)
#endif

#endif // __CALIBRATION_TABLES_H__