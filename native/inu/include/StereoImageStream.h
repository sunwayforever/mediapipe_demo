/*
* File - StereoStream.h
*
* This file is part of the Inuitive SDK
*
* Copyright (C) 2014 All rights reserved to Inuitive
*
*/

#ifndef __STEREOIMAGESTREAM_H__
#define __STEREOIMAGESTREAM_H__

#include "StreamsExport.h"

#include "ImageFrame.h"

#include <functional>

#include "BaseOutputStream.h"

namespace InuDev
{

    ////////////////////////////////////////////////////////////////////////
    /// \brief    Stereo Image frame.
    ///
    /// Role: Represents Stereo Image which is comprised of left and right sensor images.
    ///
    /// Responsibilities:
    //      1. Access to Left and Right frames separately
    ///
    ////////////////////////////////////////////////////////////////////////
    class CStereoImageFrame : public CBaseFrame
    {


    public:

        /// \brief    Constructs zero size left and right images
        INUSTREAMS_API CStereoImageFrame();

        /// \brief    Constructor.
        ///
        /// \param[in] iWidth    Image width.
        /// \param[in] iHeight    Image height.
        /// \param[in] iFormat    Image Format.
        INUSTREAMS_API CStereoImageFrame(unsigned int iWidth, unsigned int iHeight, unsigned int iImageWidth, unsigned int iImageHeight, unsigned short iStartX, unsigned short iStartY, unsigned int  iFormat);

        /// \brief    Copy constructor
        INUSTREAMS_API CStereoImageFrame(const CStereoImageFrame& input);

        /// \brief    Move constructor
        INUSTREAMS_API CStereoImageFrame(const CStereoImageFrame&& input);

        /// \brief    Assignment operator
        INUSTREAMS_API CStereoImageFrame& operator=(const CStereoImageFrame& input);

        /// \brief    move operator
        INUSTREAMS_API CStereoImageFrame& operator=(const CStereoImageFrame&& input);


        INUSTREAMS_API virtual ~CStereoImageFrame();

        const CImageFrame*   GetLeftFrame() const  {  return mLeftFrame;  }
        CImageFrame*         GetLeftFrame()        {  return mLeftFrame;  }

        const CImageFrame*   GetRightFrame() const {  return mRightFrame;  }
        CImageFrame*         GetRightFrame()       {  return mRightFrame;  }

    protected:

        /// \brief    Image received from left sensor. Equals to nullptr if left Sensor's image is not available.
        CImageFrame*  mLeftFrame;

        /// \brief    Image received from right sensor. Equals to nullptr if right Sensor's image is not available.
        CImageFrame*  mRightFrame;
    };


    ////////////////////////////////////////////////////////////////////////
    /// \brief   Interface for Stereo Image service.
    ///
    /// Role: Controls Stereo images streaming service and provides Stereo Image  image frames.
    ///
    /// Responsibilities:
    ///      1. Derives CBaseStream interface
    ///      2. Knows how to acquire one Stereo Image frame (pull)
    ///      3. Knows how to provide a continuous stream of Stereo Image frames (push)
    ///
    ////////////////////////////////////////////////////////////////////////
    class CStereoImageStream : public CBaseOutputStream
    {
    public:

        enum EOutputType
        {
            eDefaultDisplay = 0,   ///<  Provides CImageFrames of CImageFrame::EFormat::eBGRA format
            eRaw = 1,   ///<  Provides CImageFrames as are streamed by Inuitive's chip, no additional processing on Host
            eBGRA,  ///<  Provides CImageFrames of CImageFrame::EFormat::eBGRA format
            eRGBA,  ///<  Provides CImageFrames of CImageFrame::EFormat::eRGBA format
            eABGR,  ///<  Provides CImageFrames of CImageFrame::EFormat::eABGR format
            eARGB,  ///<  Provides CImageFrames of CImageFrame::EFormat::eARGB format
            eRaw8   ///<  Provides CImageFrames streamed by Inuitive's chip, no additional processing and on 8 bits.
        };

        /// \brief    Prototype of callback function which is used by the Register method.
        ///
        /// This function is invoked any time a frame is ready, or if an error occurs. The parameters of this function are:
        /// Caller stream object, received Stereo Image  frame and result code.
        typedef std::function<void(std::shared_ptr<CStereoImageStream>, std::shared_ptr<const CStereoImageFrame>,  CInuError)> CallbackFunction;

        virtual ~CStereoImageStream() {}


        using CBaseStream::Init;

        /// \brief    Service initialization.
        ///
        /// Shall be invoked once before starting frames acquisition.
        /// \param[in] EOutputType     Acquire Stereo Image  output format. default is set according to FW
        /// \return InuDev::CInuError    Error code, InDev::eOK if operation successfully completed.

        virtual CInuError         Init(EOutputType iOutputType) = 0;

        /// \brief    Retrieves new Stereo Image  image frame (pull)
        ///
        /// This method returns when a new frame is ready (blocking) or if an input timeout has elapsed.
        /// It shall be called only after a Start() was is invoked but before any invocation of a Stop() is invoked.
        /// \param[out] oStereoImageFrame    The returned Stereo Image  frame (Z-buffer).
        /// \param[in] iTimeout    Function is returned if timeout has elapsed even if no new frame is ready.
        /// \return CInuError    Error code, InDev::eOK if operation successfully completed.
        virtual CInuError GetFrame(std::shared_ptr<const CStereoImageFrame>& oStereoImageFrame, unsigned int iTimeout = FPS_BASED_TIMEOUT) = 0;

        /// \brief    Registration/Unregistration for receiving stream of Stereo Image  image frames (push)
        ///
        /// The provided callback function is called when a new frame is ready (non-blocking).
        /// It shall be called only after a Start() was is invoked but before any invocation of a Stop() is invoked.
        /// \param[in] iCallback    Callback function which is invoked when a new frame is ready.
        ///                         Send nullptr to unregister for receiving frames.
        /// \param[in] iTimeout    Callback is invoked if a timeout has elapsed even if no new frame is ready.
        /// \return CInuError    Error code, InDev::eOK if operation successfully completed.
        virtual CInuError Register(CallbackFunction iCallback, unsigned int iTimeout = FPS_BASED_TIMEOUT) = 0;

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
