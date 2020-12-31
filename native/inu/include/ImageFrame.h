/*
* File - ImageFrame.h
*
* This file is part of the Inuitive SDK
*
* Copyright (C) 2014 All rights reserved to Inuitive
*
*/

#ifndef __IMAGEFRAME_H__
#define __IMAGEFRAME_H__

#include "StreamsExport.h"

#include "GeneralFrame.h"
#include "InuSensor.h"

#include <map>

namespace InuDev
{
    struct CImageFrameParams
    {
        CImageFrameParams()
            : channelId(0)
            , frameStartX(0)
            , frameStartY(0)
            , frameWidth(0)
            , frameHeight(0)
        {}

        unsigned short  channelId;
        unsigned short  frameStartX;
        unsigned short  frameStartY;
        unsigned short  frameWidth;
        unsigned short  frameHeight;
    };

    ////////////////////////////////////////////////////////////////////////
    /// \brief    Represents  an image that is  provided  by InuDev streams
    ///
    /// Responsibilities:
    ///      1. Image attributes: format, scale, width, height and number of bytes per pixel.
    ///      2. Knows how to manage the image buffer.
    ///
    ////////////////////////////////////////////////////////////////////////
    class CImageFrame : public CGeneralFrame
    {

    public:

        ///  \brief     Supported formats
        enum EFormat
        {
            eEmpty = 0,					///<  Empty buffer
            eDepth,						///<  2 Bytes per pixel: Z-Buffer (16 bits per pixel)
            eBGR,						///<  3 Bytes per pixel: B, G and R
            eBGRA,						///<  4 Bytes per pixel: B, G, R and Alpha (which is always 0xff - 100%)
            eDisparity,					///<  2 Bytes per pixel: 4 MSB of confidence and 12 LSB of Disparities
            eRGB565,					///<  2 Bytes per pixel: Standard RGB565 format
            eDisparityB0,				///<  2 Bytes per pixel: 8 MSB of Disparity, 4 bits of sub-pixel and 4 LSB of confidence
            eRGBA,						///<  4 Bytes per pixel: R, G, B and Alpha (which is always 0xff - 100%)
            eDepthWithConfidence,		///<  2 Bytes per pixel: 14 MSB of depth and 2 LSB of confidence (the higher the better)
            ePointCloud,				///<  !Deprecated!  - 3 floats per pixel: X, Y, Z of point cloud
            eYUV422,					///<  2 bytes per pixel: Compressed Y, U, V
            eABGR,						///<  4 Bytes per pixel: Alpha, B, G, R (Alpha which is always 0xff - 100%)
            eARGB,						///<  4 Bytes per pixel: Alpha, R, G, B (Alpha which is always 0xff - 100%)
            eRGB,						///<  3 Bytes per pixel: R, G and B
            ePointCloudWithConfidence,  ///<  !Deprecated! - 4 floats per pixel: X, Y, Z of point cloud + 1 float of confidence
            eYUV422_B0,					///<  2 bytes per pixel: Compressed Y, V, U
            eDisparity9bits,			///<  2 Bytes per pixel: 9 MSB of Disparity, 3 bits of sub-pixel and 3 LSB of confidence

            eYUV = eEmpty + 100,		///<  2 Bytes per pixel: packed Y, U, V
            eY,							///<  2 Bytes per pixel: 10 bits Y (the 11th bit indicates overflow)
            eUnpackedYUV,				///<  3 Bytes per pixel: Y, U, V
            eYUVFloat,					///<  3 floats per pixel: Y, U, V
            eBayerGRBG,					///<  2 Bytes per pixel: Bayer GRBG
            eBayerRGGB,					///<  2 Bytes per pixel: Bayer RGGB
            eBayerBGGR,					///<  2 Bytes per pixel: Bayer BGGR
            eBayerGBRG,					///<  2 Bytes per pixel: Bayer GBRG
            eDepthA0,					///<  2 Bytes per pixel: Depth from HW: 5 bits of confidence (MSB)
            eDepthB0,					///<  2 Bytes per pixel: Depth from HW: 14 MSB of depth and 2 LSB of confidence (the higher the better)
            eY8,						///<  1 Bytes per pixel: 8 bit Y

            eSegmentation,              ///<  1 Bytes per pixel: Class/Label per pixel
            eDepth4000B0,                ///<  3 Bytes per pixel: Depth from HW:Packed to 20 bits represents: 16 MSB depth + 4 LSB confidence.

			eY10MSB = eEmpty + 300					///<  2 Bytes per pixel: 10 bits Y located in msb
        };

        /// \brief    Constructs empty (zero size) frame
        INUSTREAMS_API CImageFrame();

        /// \brief    Constructor.
        /// \param[in] iWidth         Image width.       (full buffer width)
        /// \param[in] iHeight        Image height.
        /// \param[in] iImageWidth    Image width.       (real size of image inside the buffer)
        /// \param[in] iImageHeight   Image height.
        /// \param[in] iStartX        X offset inside the buffer
        /// \param[in] iStartY        Y offset inside the buffer
        /// \param[in] iFormat        Image format (which defines  the number of bytes  per pixel).
        /// \param[in] iMinDisparity
        /// \param[in] iMaxDisparity
        INUSTREAMS_API CImageFrame(unsigned int iWidth,
            unsigned int iHeight,
            unsigned int iImageWidth,
            unsigned int iImageHeight,
            unsigned short iStartX,
            unsigned short iStartY,
            unsigned int iFormat,
            unsigned int iMinDisparity = 0,
            unsigned int iMaxDisparity = 0);


        /// \brief    Constructor.
        /// \param[in] iData          Data buffer.
        /// \param[in] iWidth         Image width.      (full buffer width)
        /// \param[in] iHeight        Image height.
        /// \param[in] iImageWidth    Image width.      (real size of image inside the buffer)
        /// \param[in] iImageHeight   Image height.
        /// \param[in] iStartX        X offset inside the buffer
        /// \param[in] iStartY        Y offset inside the buffer
        /// \param[in] iFormat    Image format (which defines  the number of bytes  per pixel).
        /// \param[in] iOwnData    If this flag is true then this CImageFrame object is responsible for freeing the buffer
        INUSTREAMS_API CImageFrame(InuDev::byte*  iData,
            unsigned int iWidth,
            unsigned int iHeight,
            unsigned int iImageWidth,
            unsigned int iImageHeight,
            unsigned short iStartX,
            unsigned short iStartY,
            unsigned int iFormat,
            bool iOwnData = true);

        /// \brief    Destructor.
        INUSTREAMS_API virtual ~CImageFrame() {}

        /// \brief    Image Size - number of pixels
        unsigned int ImageSize() const { return mWidth * mHeight; }

        /// \brief    Image Size - number of pixels
        unsigned int BufferSize() const { return GetDataSize(); }

        /// \brief    Image width
        unsigned int Width() const { return mWidth; }

        /// \brief    Image height
        unsigned int Height() const { return mHeight; }

        /// \brief    Width of image that is cropped by depth engine, is relevant when the image padding is requested. CroppedImageWidth <=  Width
        unsigned int CroppedImageWidth() const { return mCroppedImageWidth; }

        /// \brief    Height of image  that is cropped by depth engine, is relevant when the image padding is requested. CroppedImageHeight <=  Height
        unsigned int CroppedImageHeight() const { return mCroppedImageHeight; }

        /// \brief    Top left width offset of cropped image, is relevant when the image padding is requested. CroppedImageTopLeftW >= 0
        unsigned short CroppedImageTopLeftW() const { return mCroppedImageTopLeftW; }

        /// \brief    Top left height offset of cropped image, is relevant when the image padding is requested. CroppedImageTopLeftH >= 0
        unsigned short CroppedImageTopLeftH() const { return mCroppedImageTopLeftH; }

        /// \brief    Image format
        unsigned int Format() const    { return mFormat; }

		/// \brief  Set width
		void SetFormat(unsigned int iFormat) { mFormat = iFormat; }

		/// \brief  Set width
		void SetWidth(unsigned int iWidth) { mWidth = iWidth; }

		/// \brief  Set width
		void SetHeight(unsigned int iHeight) { mHeight = iHeight; }

		/// \brief  Set CroppedImageWidth
		void SetCroppedImageWidth(unsigned int iCroppedImageWidth) { mCroppedImageWidth = iCroppedImageWidth; }

		/// \brief  Set CroppedImageHeight
		void SetCroppedImageHeight(unsigned int iCroppedImageHeight) { mCroppedImageHeight = iCroppedImageHeight; }

		void SetCroppedImageTopLeftW(unsigned int iCroppedImageTopLeftW) { mCroppedImageTopLeftW = iCroppedImageTopLeftW; }

		void SetCroppedImageTopLeftH(unsigned int iCroppedImageTopLeftH) { mCroppedImageTopLeftH = iCroppedImageTopLeftH; }


        /// \brief    Number of bytes that are used to represent each pixel.
        INUSTREAMS_API unsigned int BytesPerPixel() const;

        /// \brief    Exposure parameters while this image was grabbed. The key refers to the sensor ID.
        const std::map<uint16_t, CExposureParams>& GetSensorsExposureParams() const { return mExposureParams; }

        /// \brief    Returns read out time.
        const uint64_t GetReadoutTime() const { return mReadoutTime; }

        /// \brief    Number of invalid pixels received by depth calculation
        uint32_t GetNumOfInvalidDepthPixels() const { return  mNumOfInvalidDepthPixels; }

        /// \brief      Set depth scale value as 1mm/depthscale      
        unsigned int  GetDepthScale() const { return mDepthScale; }

        /// \brief      Set Depth bit starting position(from iDepthPosition to MSB) 
        unsigned int  GetDepthPosition() const { return mDepthPosition; }

    protected:

        /// \brief     Image width (full buffer width)
        unsigned int mWidth;

        /// \brief     Image height (full buffer height)
        unsigned int mHeight;

        /// \brief    Width of image that is cropped by depth engine, is relevant when the image padding is requested. CroppedImageWidth <= Width
        unsigned int mCroppedImageWidth;

        /// \brief    Height of image  that is cropped by depth engine, is relevant when the image padding is requested. CroppedImageHeight <= Height
        unsigned int mCroppedImageHeight;

        /// \brief    Top left width offset of cropped image, is relevant when the image padding is requested. CroppedImageTopLeftW >= 0
        unsigned short mCroppedImageTopLeftW;

        /// \brief    Top left height offset of cropped image, is relevant when the image padding is requested. CroppedImageTopLeftH >= 0
        unsigned short mCroppedImageTopLeftH;

        /// \brief     Image Format
        unsigned int mFormat;

        /// \brief    Holds the ET, digital and analog gain for one stream. The key refers to the sensor ID.
        std::map<uint16_t, CExposureParams> mExposureParams;

        /// \brief    Holds the raed out time in nano seconds. Read out time is the time in which the pixels from the sensor starts to recieved in the chip
        uint64_t mReadoutTime;

        /// \brief    Number of invalid pixels received by depth calculation
        uint32_t  mNumOfInvalidDepthPixels;

        unsigned int mMinDisparity;

        unsigned int mMaxDisparity;

        /// \brief    Depth scale factor: 1mm/scalefactor as Z quantization resolution.
        unsigned int mDepthScale;

        /// \brief    Depth representation start from this bit to MSB.
        unsigned int mDepthPosition;

        /// \brief    Number of Interleaved images in the frame.
        unsigned int mNumInterleavedImages;
    };
}

#endif // __IMAGEFRAME_H__
