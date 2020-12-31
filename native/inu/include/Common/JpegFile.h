#pragma once

#include "CommonUtilitiesDefs.h"
#include <string>

namespace InuCommon
{
    ////////////////////////////////////////////////////////////////////////
    /// \brief    Interface for JPEG files 
    ///
    /// Role: Utility class which provides interface for reading and writing
    ///       JPEG files format.
    ///
    /// Responsibilities: 
    ///      1. Read JPEG files to a buffer
    ///      2. Write buffer to a JPEG file
    ///
    /// Limitation: Only supports 8 bit, 3 channels JPEG files (most common type)
    ////////////////////////////////////////////////////////////////////////
    class COMMONUTILITIES_API CJpegFile
    {
    public:

        /// \brief    Returned error codes
        enum EError
        {
            eOK = 0,
            eFileOpen,          /// Can't open file - make sure it exists, accessible and formatted as JPEG.
            eWriteData,         /// Can't create JPEG file out of data - make sure it sane. 
            eUnsupportedType,   /// Frame does not match the supported type (8-bit, 3 channels)
        };

        /// \brief    Save input image data as JPEG file
        /// \param[in] iFilePath          Output file path.
        /// \param[in] iBuffer            Buffer containing an 8-bit RGB JPEG frame.
        /// \param[in] iWidth             Width of frame (in pixels).
        /// \param[in] iHeight            Height of frame (in pixels).
        /// \param[in] iQuality           level of compression - from 0 to 100 (the higher is the better).
        /// \pre iBuffer matches to a iwidth * iHeight 8-bit RGB matrix
        ///      iQuality is in the range 0 to 100.
        ///      the directory of iFilePath exists and the caller has sufficient permissions for creating a file in it
        /// if the pre condition is not being held you should expect unexpected behavior.
        /// \return eOK in case of success, and eWriteData otherwise.
        static EError Write(const std::string& iFilePath,
            const unsigned char* const iBuffer,
            int iwidth,
            int iHeight,
            int iQuality);

        /// \brief    Retrieve image data and attributes from input JPEG file.
        /// \param[in]  iFilePath         Input file path.
        /// \param[out] oBuffer           buffer to be filled with an 8-bit RGB JPEG frame.
        /// \param[out] oSizeOfBuffer     size of buffer (in bytes).
        /// \param[out] oWidth            width of frame (in pixels).
        /// \param[out] oHeight           Height of frame (in pixels).
        /// \pre file is a valid RGB JPEG file, accessible by the caller.  
        /// \return eOK in case of success, and error code otherwise.
        static EError Read(const std::string& iFilePath,
            unsigned char*& oBuffer,
            int& oSizeOfBuffer,
            int& oWidth,
            int& oHeight);

    private:
        static const int mTypeOfMatrix;
        
    };
}
