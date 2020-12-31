/*
 * File - CFeaturesTrackingFrame.h
 *
 * This file is part of the Inuitive SDK
 *
 * Copyright (C) 2014 All rights reserved to Inuitive
 *
 */

#ifndef __FEATURESTRACKINGFRAME_H__
#define __FEATURESTRACKINGFRAME_H__

#include "GeneralFrame.h"

namespace InuDev {
////////////////////////////////////////////////////////////////////////
/// \brief    Represents generic buffer that is provided by some of InuDev
/// streams
///
/// Responsibilities:
///      1. Buffer attributes: data and size.
///      2. Memory management control.
///
/// Comment: The interpretation of provided buffers should be done by the caller
/// application.
///          The caller application should be familiar with the internal format
///          of each provided frame.
////////////////////////////////////////////////////////////////////////

class CFeaturesTrackingFrame : public CGeneralFrame {
   public:
    enum EOutputType { eRaw, eParsed, eProcessed };

    struct CParsedData {
        /// \brief 512 bits – FREAK / LIFT binary descriptor
        uint32_t Descriptor[16];  // descriptor - 512 bit

        /// \brief Subpixel keypoint position (mX, mY) [pixels * 2^14] (divide
        /// it by 2^14 to get the X,Y position in subpixel) In Hybrid mode, if
        /// the inputs are 2 images concatenated one next to the other, the sub
        /// pixel position is relative to the starting point of each image
        /// (parameter mNotImg1 below tells you if the keypoint is from the left
        /// image or from the right image
        float X;
        float Y;

        /// \brief Keypoint angle - Anti-clockwise, [in radians]
        float Angle;

        /// \brief Radius around the point
        /// Radius around the point (in pixels). To be used for the descriptor
        /// calculation - you can extract the keypoint scale using this formula:
        /// scale = 2/log(2)*log((mPatternSize – 1)/16) (or with a lookup table)
        uint32_t PatternSize;

        /// \brief IsRightImage
        /// If the inputs are 2 images concatenated one next to the other
        /// Keypoints can be from the left image or from the right image
        bool IsRightImage;

        CParsedData();
    };

    struct CProcessedData : public CParsedData {
        /// \brief  Unique Id for the keypoint.
        /// If the keypoint has a match to another keypoint in the next frame
        /// its Id stays the same
        uint32_t UniqId;

        /// \brief   Number of sequential frames in which this keypoint was
        /// detected
        uint32_t Confidence;

        CProcessedData() : UniqId(0), Confidence(0) {}
    };

   public:
    /// \brief    Constructs empty (zero size) buffer
    INUSTREAMS_API CFeaturesTrackingFrame();

    /// \brief    Constructor.
    /// \param[in] bufferSize    generated buffer size.
    INUSTREAMS_API CFeaturesTrackingFrame(uint32_t iBufferSize)
        : CGeneralFrame(iBufferSize) {}

    /// \brief    Destructor.
    INUSTREAMS_API virtual ~CFeaturesTrackingFrame() {}

    /// \brief    Getter for frame data
    INUSTREAMS_API const InuDev::byte* GetRawData() const;

    /// \brief    Getter for frame data
    INUSTREAMS_API CParsedData* GetParsedData() const;

    /// \brief    Getter for frame data
    INUSTREAMS_API CProcessedData* GetProcessedData() const;

    /// \brief    Getter for Image Width
    INUSTREAMS_API uint16_t GetImageWidth() const;

    /// \brief    Getter for Image Height
    INUSTREAMS_API uint16_t GetImageHeight() const;

    /// \brief    Getter for OutputType
    INUSTREAMS_API EOutputType GetOutputType() const;

    /// \brief    Getter for OutputType
    INUSTREAMS_API uint32_t GetKeyPointNumber() const;

   protected:
    /// \brief    OutputType - maybe  eRaw eParsed or eProcessed
    EOutputType mOutputType;

    /// \brief  Processed  Image dimentions
    uint16_t mImageWidth;
    uint16_t mImageHeight;

    /// \brief  Number of Key Points in the Processed Image
    uint32_t mKPNumber;
};
}  // namespace InuDev

#endif  // __FEATURESTRACKINGFRAME_H__
