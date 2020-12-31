/*
* File - CnnAppStream.h
*
* This file is part of the Inuitive SDK
*
* Copyright (C) 2014 All rights reserved to Inuitive
*
*/

#ifndef __CNNAPPSTREAMH__
#define __CNNAPPSTREAMH__

#include "StreamsExport.h"
#include "BaseOutputStream.h"
#include "CnnAppFrame.h"

#include <functional>
#include <string>
#include <vector>

namespace InuDev
{
    ////////////////////////////////////////////////////////////////////////
    /// \brief    CNN stream which allows: Face recognition, Object detection, Segmentation and Classification
    ///
    /// Role: AI services and provides AI output according to the activated network.  
    ///
    /// Responsibilities: 
    ///      1. Knows how to control the service (Init, Terminate, Start and Stop).
    ///      2. Knows how to acquire one frame of all AI information (pull)
    ///      3. Knows how to provide a continuous stream of frames of all AI information (push)
    ///
    ////////////////////////////////////////////////////////////////////////
    class CCnnAppStream : public CBaseOutputStream
    {
    public:

        typedef std::function<void(std::shared_ptr<CCnnAppStream>, std::shared_ptr<const CCnnAppFrame>, CInuError)> CallbackFunction;

        virtual ~CCnnAppStream() {}

        /// \brief    Derived interface, Init without initial parameters
        using CBaseStream::Init;

        /// \brief    Service initialization.
        /// 
        /// Shall be invoked once before starting frames acquisition.
        /// \param[in] iType CNN App output type: determines the specific AI network type to run.
        /// \return InuDev::CInuError    Error code, InDev::eOK if operation successfully completed.
        virtual CInuError         Init(CCnnAppFrame::EOutputType iType) = 0;

        virtual CInuError         Start(std::string iNetworkPath = std::string(), std::string iNetworkFunctionName = std::string()) = 0;

        virtual CInuError         GetFrame(std::shared_ptr<const CCnnAppFrame>& oCnnAppFrame, unsigned int iTimeout = FPS_BASED_TIMEOUT) = 0;

        virtual CInuError         Register(CallbackFunction iCallback, unsigned int iTimeout = FPS_BASED_TIMEOUT) = 0;

        /// \brief    Get segmentatin labels for segmentation image
        /// \param[out] oSegmentationLabels Labels returned from AI lib.
        /// \return InuDev::CInuError    Error code, InDev::eOK if operation successfully completed.
        virtual CInuError         GetSegmentationLabels(std::map<uint32_t, std::string>& oSegmentationLabels) = 0;

        /// \brief    Enroll new person(face) for Face recognition AI, can be used only after Enroll stream is initialized and started.
        /// \param[in] iName - Name of the person to enroll
        /// \param[in] iPath - Path to folder with images to enroll, default images will be taken from "C:\Program Files\Inuitive\InuDev\config\AI\FaceDetection\faces"
        /// \return InuDev::CInuError    Error code, InDev::eOK if operation successfully completed.
        virtual CInuError         EnrollPerson(const std::string& iName, const std::string& iPath = std::string()) = 0;

        /// \brief    Delete person(face) from Face recognition AI Database, can be used only after Enroll stream is initialized and started.
        /// \param[in] iName - Name of the person to enroll
        /// \return InuDev::CInuError    Error code, InDev::eOK if operation successfully completed.
        virtual CInuError         DeletePerson(const std::string& iName) = 0;

        /// \brief    Retrieve vector of names from face recognition AI Database, can be used only after Enroll stream is initialized and started.
        /// \param[in] oName - Names of enrollees
        /// \return InuDev::CInuError    Error code, InDev::eOK if operation successfully completed.
        virtual CInuError         GetListOfFaces(std::vector<std::string>& oNames) = 0;

        /// \brief    Retrieve Image channel ID connected to the CNNApp Stream. Can only be called after CnnAppStream is started.
        /// \param[in] oChannelID - Image channel ID as set by default or by user.
        /// \return InuDev::CInuError    Error code, InDev::eOK if operation successfully completed, Error returned if stream not started.
        virtual CInuError         GetImageChannel(uint32_t& oChannelID) = 0;

    };
}

#endif // __ObjectsDetectionSTREAM_H__
