/*
* File - ImageStream.h
*
* This file is part of the Inuitive SDK
*
* Copyright (C) 2014 All rights reserved to Inuitive
*
*/

#ifndef __IMAGESTREAM_H__
#define __IMAGESTREAM_H__

#include "StreamsExport.h"

#include "ImageFrame.h"

#include <functional>

namespace InuDev
{

    ////////////////////////////////////////////////////////////////////////
    /// \brief   Interface for Web cam service.
    ///
    /// Role: Controls web cam images streaming service and provides web cam image frames.
    ///
    /// Responsibilities:
    ///      1. Derives CBaseStream interface
    ///      2. Knows how to acquire one web cam image frame (pull)
    ///      3. Knows how to provide a continuous stream of web cam image frames (push)
    ///
    ////////////////////////////////////////////////////////////////////////
    class CImageStream : public CBaseOutputStream
    {
    public:

        enum EOutputType
        {
            eDefault= 0,  	///<  Provides CImageFrames of CImageFrame::EFormat::eRGBA format
            eRegisteredImage = 1,  ///<  Projection of image on depth, format is similar to eDefault
            eUndistortedImage = 2,  ///<  Undistorted image, format is similar to eDefault
            eRaw = 3, 	///<  Provides CImageFrames as are streamed by Inuitive's chip, no additional processing on Host
            eBGR = 4 ///< Provides CImageFrame of CImageFrame::EFormat::BGR format
        };

        /// \brief    Prototype of callback function which is used by the Register method.
        ///
        /// This function is invoked any time a frame is ready, or if an error occurs. The parameters of this function are:
        /// Caller stream object, received web cam frame and result code.
        typedef std::function<void(std::shared_ptr<CImageStream>, std::shared_ptr<const CImageFrame>,  CInuError)> CallbackFunction;

        virtual ~CImageStream() {}

        /// \brief    Retrieves new web cam image frame (pull)
        ///
        /// This method returns when a new frame is ready (blocking) or if an input timeout has elapsed.
        /// It shall be called only after a Start() was is invoked but before any invocation of a Stop() is invoked.
        /// \param[out] oImageFrame    The returned web cam frame (Z-buffer).
        /// \param[in] iTimeout    Function is returned if timeout has elapsed even if no new frame is ready.
        /// \return CInuError    Error code, InDev::eOK if operation successfully completed.
        virtual CInuError GetFrame(std::shared_ptr<const CImageFrame>& oImageFrame, unsigned int iTimeout = FPS_BASED_TIMEOUT) = 0;

        /// \brief    Registration/Unregistration for receiving stream of web cam image frames (push)
        ///
        /// The provided callback function is called when a new frame is ready (non-blocking).
        /// It shall be called only after a Start() was is invoked but before any invocation of a Stop() is invoked.
        /// \param[in] iCallback    Callback function which is invoked when a new frame is ready.
        ///                         Send nullptr to unregister for receiving frames.
        /// \param[in] iTimeout    Callback is invoked if a timeout has elapsed even if no new frame is ready.
        /// \return CInuError    Error code, InDev::eOK if operation successfully completed.
        virtual CInuError Register(CallbackFunction iCallback, unsigned int iTimeout = FPS_BASED_TIMEOUT) = 0;

        /// \brief    Don't mask the default init function (which disables registration by default)
        using CBaseStream::Init;

        /// \brief    Overwrite the default Initialization method
        ///
        /// It enables to activate registration between Image and Depth.
        /// It must be set to true in order to activate registration by SetParams method.
        /// The side effect of enabling registration is that Depth stream will be automatically started whenever Image stream is started.
		/// \param[in] iOutputType    O utput format of acquired frames.
		/// \param[in] iRegChannelID     Register Channel ID - in the case of  eRegisteredImage only
		/// \return CInuError
        virtual CInuError Init(EOutputType iOutputType, uint32_t iRegChannelID = DEFAULT_CHANNEL_ID) =0;
        

        /// \brief    Moves the cropping region of interest in runtime
        ///
        /// Enables to move the region of interest rectangle that was set using the InuSensor::SetChannelCropping before starting the device.
        /// In case InuSensor::SetChannelCropping wasn't called the operation will fail.
        /// Moving the rectangle outside the bouderies of the viewable area will cause the image to freeze.
        /// \param[in] uint32_t iStartX, uint32_t iStartY: sets the position of the upper left corner of the rectangle
        /// \return CInuError
        virtual CInuError SetCroppingROI(uint32_t iStartX, uint32_t iStartY) = 0;
    };
}

#endif
