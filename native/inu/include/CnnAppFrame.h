/*
* File - CnnAppFrame.h
*
* This file is part of the Inuitive SDK
*
* Copyright (C) 2014 All rights reserved to Inuitive
*
*/

#ifndef __CNNAPPFRAME_H__
#define __CNNAPPFRAME_H__


#include "ImageFrame.h"
#include "GeneralFrame.h"

namespace InuDev
{
    ////////////////////////////////////////////////////////////////////////
    /// \brief    Represents generic buffer that is provided by some of InuDev streams
    ///
    /// Responsibilities:
    ///      1. Buffer attributes: data and size.
    ///      2. Memory management control.
    ///
    /// Comment: The interpretation of provided buffers should be done by the caller application.
    ///          The caller application should be familiar with the internal format of
    ///          each provided frame.
    ////////////////////////////////////////////////////////////////////////

    class CCnnAppFrame : public CGeneralFrame
    {

    public:   
        enum EOutputType
        {
			/// \brief		Object detection SSD fast but less accurate
            eObjectDetection,
            eSegmentation,
            eClassification,
            eFaceRecognition,
			/// \brief		Similar behavior as eObjectDetection but a different network(YoloV3) slower & accurate.
            eObjectDetectionYoloV3,
            eEnrollFace
        };

        struct CDetectedObject
        {
            /// \brief		Classification identifier of this object
            std::string ClassID;

            /// \brief		Confidence score of this object
            float Confidence;

            /// \brief		Top Left coordinates of detected object.
            CPoint2D ClosedRectTopLeft;

            /// \brief		Size of recognized face.
            CPoint2D ClosedRectSize;

        };

        struct CClassificationData
        {
            /// \brief		Classification identifier of this object
            std::string ClassID;

            /// \brief		Confidence score of this object
            float Confidence;
        };

        /// \brief	Face position
        enum EFacePose
        {
            eNone,		///< Unrecognized position
            eLeft,		///< Looking left (Horizontal
            eRight,		///< Looking right (Horizontal)
            eTop,		///< Looking top (Vertical)
            eBottom,	///< Looking bottom (Vertical)
            eCenter		///< Looking to the center (Horizontal/Vertical
        };

        struct CRecognizedFace
        {
            /// \brief	Classification identifier of this object
            std::string FaceID;

            /// \brief	Confidence score of this object
            float Confidence;

            /// \brief	Top Left coordinates of detected object.
            CPoint2D ClosedRectTopLeft;

            /// \brief	Size of recognized face.
            CPoint2D ClosedRectSize;

            /// \brief	Horizontal face position (Left/Right/Center)
            EFacePose PoseH;

            /// \brief	Vertical face position (Top/Bottom/Center)
            EFacePose PoseV;

            /// \brief	Number of landmark points
            static const int LANDMARKS_POINTS = 5;

            /// \brief	landmark points (eyes, nose, mouth)
            CPoint<2, uint32_t> Landmarks[LANDMARKS_POINTS];
        };

    public:

        /// \brief    Constructs empty (zero size) buffer
        INUSTREAMS_API CCnnAppFrame();

        /// \brief    Constructor.
        /// \param[in] bufferSize    generated buffer size.
        INUSTREAMS_API CCnnAppFrame(uint32_t iBufferSize) : CGeneralFrame(iBufferSize){}

        /// \brief    Destructor.
        INUSTREAMS_API virtual ~CCnnAppFrame() {}

        /// \brief    Getter for face recognition frame data
        INUSTREAMS_API std::shared_ptr<std::vector<CRecognizedFace>> GetFaceData() const;

        /// \brief    Getter for object detection frame data
        INUSTREAMS_API std::shared_ptr<std::vector<CDetectedObject>> GetObjectData() const;

        /// \brief    Getter for classification frame data
        INUSTREAMS_API std::shared_ptr<CClassificationData>  GetClassificationData() const;

        /// \brief    Getter for segmentation frame data
        INUSTREAMS_API std::shared_ptr<CImageFrame>  GetSegmentationData() const;

        /// \brief    Getter for CnnType
        INUSTREAMS_API EOutputType GetOutputType() const;

        /// \brief    Getter for enrollment status
        INUSTREAMS_API CInuError GetEnrollmentStatus() const;

    protected:

        /// \brief      CnnType - Object/Face/Segmentation/Classification
        EOutputType mOutputType;

        /// \brief		Collection of all detected objects
        std::shared_ptr<std::vector<CDetectedObject>> mDetectedObjects;

        /// \brief		Collection of all detected objects
        std::shared_ptr<std::vector<CRecognizedFace>> mDetectedFaces;

        /// \brief		Classification data
        std::shared_ptr<CClassificationData> mClassificationData;

        /// \brief		Segmentation data 
        std::shared_ptr<CImageFrame> mSegmentationData;

        /// \brief		Error representing enrollment status
        int mEnrollmentStatus;

    private:

    };
}

#endif // __CNNAPPFRAME_H__
