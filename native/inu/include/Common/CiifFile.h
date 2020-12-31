#ifndef __CIIF_FILE_H_
#define __CIIF_FILE_H_

#include "CommonUtilitiesDefs.h"
#include "OSUtilities.h"
#include "ImageFile.h"

#include <string>
#include <vector>
#include <map>

#define  CIFF_PRINT_SIZE 4
#define  MAX_FORMAT_SIZE 20
#define  MAX_LINE_LENGTH 600
#define  MAX_BUFFER_SIZE 400*MAX_LINE_LENGTH

#ifdef _MSC_VER
#pragma warning(disable : 4251)
#endif

namespace InuCommon
{
    ////////////////////////////////////////////////////////////////////////
    /// \brief    Interface for Ciif files 
    ///
    /// Role: Utility class which provides interface for reading and writing
    ///       Inuitive's images in Ciif files format.
    ///
    /// Responsibilities: 
    ///      1. Pack and save Inuitive's image as Ciif file
    ///      2. Read header, data or both 
    ////////////////////////////////////////////////////////////////////////
    class COMMONUTILITIES_API CCiifFile
    {
    public:

        /// \brief    Returned error codes
        enum EError
        {
            eOK = 0,
            eFileOpen,          /// Can't open input tiff file 
            eUnexpectedFormat,  /// Input file format is different than ECiifFormat
            eInvalidFileName,   /// Input file name is not valid
            eFailedWritingData, /// Failed to write the data  
        };

        enum ELineDirection
        {
            eLeftToRight,
            eRightToLeft
        };

        enum EDataFormat
        {
            eHex
        };

        enum ECiifFormat
        {
            DEFAULT = -1,
            // RGB              
            RGB24, // 24 bit output            
            // YUV            
            YUV24,
            // RAW
            RAW8,
            RAW12,
            RAW16,
            RAW24, // 24 bit output
            RAW32, // 32 bit output
            RAW64, // 64 bit output			
            NUM_OF_PIXEL_FORMATS
        };       

        static std::map<std::string, ECiifFormat> mPixelFormatMapper;

        // constants from CSE\NU3000Sim\Global\DataFormats.h
        static const int mBytesPerPixel[NUM_OF_PIXEL_FORMATS];
        static const int mNibblesPerPixel[NUM_OF_PIXEL_FORMATS];
        static const  std::string mHexOutWidth[];
        static std::map<ELineDirection, std::string> mLineDirectionString;

        /// \brief    Additional attributes that can be saved in the tiff file together with image data
        struct COMMONUTILITIES_API CMetaData
        {     

        public:

            int mWidth;
            int mHeight;
            int mNumberDisparities;
            int mNumberOfFrames;
            ECiifFormat mPixelFormat;            
            EDataFormat mDataFormat;
            char mExportFormat[MAX_FORMAT_SIZE];
            unsigned int mBytesPerPixel;

            CMetaData(){};

            CMetaData(
                int iWidth,
                int iHeight,
                int iNumberDisparities,
                int iNumberOfFrames,
                ECiifFormat iPixelFormat,
                ELineDirection iLineDirection,
                EDataFormat iDataFormat,
                const char iExportFormat[MAX_FORMAT_SIZE],
                unsigned int iBytesPerPixel);

            std::string GetLineDirection() const { return mLineDirectionString[mLineDirection]; }
            void SetLineDirection(std::string direction) {  mLineDirection = (direction == mLineDirectionString[eLeftToRight]) ? eLeftToRight : eRightToLeft; }
                   
        private:

            ELineDirection mLineDirection;
        };

        /// \brief    Save input image data as Tiff file
        ///
        /// \param[in] iFileName    Output file name.
        /// \param[in] iBuffer      Data buffer.
        /// \param[in] iMetaData    Meta data of the file 
        /// \return InuCommon::CCiifFile::EError    eOK in success or error code.
        static EError Write(            
            const std::string& iFileName,
            const unsigned char* iDataList,
            CMetaData& iMetaData,
            PixelSerializer iPixelSerializer = nullptr,
            bool iWriteHeader = true);

        /// \brief    Retrieve image data and attributes from input fiff file
        ///
        /// \param[in] iFileName    Input file name.
        /// \param[in] iMetaData
        /// \param[in] DeSerializer
        /// \param[out] oBuffer      If oBuffer is nullptr then it will be allocated and initialized by this method.		    
        /// \return InuCommon::CTiffFile::EError    eOK in success or error code.
        static EError Read(
            const std::string& iFileName,
            CMetaData& iMetaData,			
            unsigned char*& oBuffer,
            PixelDeserializer iPixelDeserializer = nullptr);

        // Convert an int to hex string padded with leading zeros (optimized for common case)
        static void PrintHexPadded(char * dest, uint64_t val, uint64_t length);


        private:

            /// \brief    Retrieve image attributes from input tiff header
            /// \param[out] iFileName    Input file name.
            /// \param[out] oWidth    
            /// \param[out] oHeight    
            /// \param[out] oBytesPerPixel
            /// \param[out] oMetaData
            /// \return InuCommon::CTiffFile::EError    eOK in success or error code.
            static EError ReadHeader(
                std::ifstream &iFile,
                CMetaData& oMetaData);

            /// \brief    Write image data and attributes to an output CIIF file
            ///
            /// \param[in] iFileName      CIIF file name.
            /// \param[in] iMetaData      Additional attributes
            /// \param[in] iFrameCounter  the frame index that should be write to the CIIF file (first image index is one, the second image index is two etc.)  
            /// \param[in] iDataPointer   pointer to the image data that should be exported to file       
            /// \return InuCommon::CTiffFile::EError    eOK in success or error code.
            static void ExportCIIF(
                const std::string &iFileName,
                const CMetaData& iMetaData,
                int iFrameCounter,
                const unsigned char* iDataPointer,
                bool iWriteHeader);

            /// \brief    Write image data and attributes to an output CIIF file
            ///
            /// \param[in] iFileName      CIIF file name.
            /// \param[in] iMetaData      Additional attributes
            /// \param[in] iFrameCounter  the frame index that should be write to the CIIF file (first image index is one, the second image index is two etc.)  
            /// \param[in] iDataPointer   pointer to the image data that should be exported to file       
            /// \return InuCommon::CTiffFile::EError    eOK in success or error code.
            static void ExportCIIF(
                const std::string &iFileName,
                const CMetaData& iMetaData,
                int iFrameCounter,
                const unsigned char* iDataPointer,
                bool iWriteHeader,
                PixelSerializer iPixelSerializer);

            static void WriteHeader(
                std::ofstream &oOutputFile,
                const CMetaData& iMetaData);

            static void WriteHeader(
                std::ofstream &oOutputFile, 
                const CMetaData& iMetaData,
                int (&datwid)[4], 
                int (&datshift)[4], 
                uint64_t (&datand)[4], 
                int (&iciiformat)[4],
                std::string iCiifstr,
                int ciif_desc_size);

            static int BuildCiifFormatDescriptor(
                const int *p_ciiformat, 
                int (&_datwid)[4], 
                int (&_datshift)[4], 
                uint64_t (&_datand)[4], 
                int (&_iciiformat)[4], 
                std::string &_ciifstr);
            
            static EError ReadData(
                std::ifstream &iFile,
                unsigned char*& oBuffer,
                unsigned int iWidth,
                unsigned int iHeight,
                unsigned int iBytesPerPixel,
                unsigned int iNumberOfFrames,
                PixelDeserializer iPixelDeserializer);

            static int StringToHex(const std::string &iHexStr, unsigned char* oBuffer);
    };
}

#ifdef _MSC_VER
#pragma warning(default : 4251)
#endif


#endif