#pragma once

#include "CommonUtilitiesDefs.h"

#include <string>

namespace InuCommon
{
    ////////////////////////////////////////////////////////////////////////
    /// \brief    Interface for PNG files 
    ///
    /// Role: Utility class which provides interface for reading and writing
    ///       PNG files format.
    ///
    /// Responsibilities: 
    ///      1. Pack and save PNG file
    ///      2. Read data
    ////////////////////////////////////////////////////////////////////////
    class COMMONUTILITIES_API CPngFile
    {
    public:

        /// \brief    Returned error codes
        enum EError
        {
            eOK = 0,
            eFileOpen,          /// Can't open input PNG file
            eWriteData,         /// Error in writing image buffer 
            eReadData,          /// Error in reading image buffer 
            eUnexpectedFormat   /// Input file format is different than RGBA
        };

        /// \brief    Save input image data as Tiff file
        /// \param[in] iFileName    Output file name.
        /// \param[in] iBuffer    Data buffer.
        /// \param[in] iWidth    
        /// \param[in] iHeight    
        /// \param[in] iFormat    
        /// \param[in] iMetaData    Additional information
        /// \param[in] iIndex    If index != -1 and iFileName doesn't end with TIFF_FILE_EXT then this index is concatenate to iFileName 
        /// \return InuCommon::CTiffFile::EError    eOK in success or error code.
        static EError Write(const std::string& iFileName,
            const unsigned char* iBuffer,
            unsigned int iWidth,
            unsigned int iHeight,
            unsigned int iBytePerPixel,
            int iIndex = -1);


        /// \brief    Retrieve image data and attributes from input tiff file
        /// \param[out] oFileName    Input file name.
        /// \param[out] oBuffer    If oBuffer is nullptr then it will be allocated and initialized by this method.
        /// \param[out] oWidth    
        /// \param[out] oHeight    
        /// \param[out] oBytesPerPixel    
        /// \param[out] oFormat    
        /// \param[out] oMetaData    
        /// \return InuCommon::CTiffFile::EError    eOK in success or error code.
        static EError Read(const std::string& iFileName,
            unsigned char*& oBuffer,
            unsigned int& oWidth,
            unsigned int& oHeight);
    };
}
