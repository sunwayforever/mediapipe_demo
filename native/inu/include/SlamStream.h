/*
* File - SlamStream.h
*
* This file is part of the Inuitive SDK
*
* Copyright (C) 2014 All rights reserved to Inuitive
*
*/

#ifndef __SLAMSTREAM_H__
#define __SLAMSTREAM_H__

#include "StreamsExport.h"

#include "BaseOutputStream.h"
#include <functional>

#define  TRANSFORM_MATRIX_SIZE 16
namespace InuDev
{
    ////////////////////////////////////////////////////////////////////////
    /// \brief  Represents frames that are provided by CSlamStream
    ////////////////////////////////////////////////////////////////////////
    struct CSlamFrame : public CBaseFrame
    {
        /// \brief    Camera location in (x, y, z) relative to an initial position in first frame
        float Translation3D[3];

        /// \brief    Camera rotation in a quaternion format relative to an initial position in first frame (in Radians)
        float Quaternion[4];

        /// \brief    Camera rotation[3x3] matrix and translation[3x1] matrix in one 4x4 matrix. Is created from Translation3D and Quaternion.
        float TransformMatrix[TRANSFORM_MATRIX_SIZE];
    };


    ////////////////////////////////////////////////////////////////////////
    /// \brief   Interface for SLAM service.
    ///
    /// Role: Controls Slam streaming service and provides Slam frames.
    ///
    /// Responsibilities:
    ///      1. Derives CBaseStream interface
    ///      2. Knows how to acquire one Slam frame (pull)
    ///      3. Knows how to provide a continuous stream of Slam frames (push)
    ///
    ////////////////////////////////////////////////////////////////////////
    class CSlamStream : public CBaseOutputStream
    {
    public:

        /// \brief    Prototype of callback function which is used by the Register method.
        ///
        /// This function is invoked any time a frame is ready, or if an error occurs. The parameters of this function are:
        /// Caller stream object, received Slam frame and result code.
        typedef std::function<void(std::shared_ptr<CSlamStream>, std::shared_ptr<const CSlamFrame>,  CInuError)> CallbackFunction;

        virtual ~CSlamStream() {}

        /// \brief    Retrieve one Slam frame (pull)
        ///
        /// This method returns when a new frame is ready (blocking) or if an input timeout has elapsed.
        /// It shall be called only after a Start() was is invoked but before any invocation of a Stop() is invoked.
        /// \param[out] oSlamFrame    The returned Slam frame.
        /// \param[in] iTimeout    Function is returned if timeout has elapsed even if no new frame is ready.
        /// \return CInuError    Error code, InDev::eOK if operation successfully completed.
        virtual CInuError GetFrame(std::shared_ptr<const CSlamFrame>& oSlamFrame, unsigned int iTimeout = FPS_BASED_TIMEOUT) = 0;

        /// \brief    Registration/Unregistration for receiving Slam frames (push)
        ///
        /// The provided callback function is called when a new frame is ready (non-blocking).
        /// It shall be called only after a Start() was is invoked but before any invocation of a Stop() is invoked.
        /// \param[in] iCallback    Callback function which is invoked when a new frame is ready.
        ///                         Send nullptr to unregister for receiving frames.
        /// \param[in] iTimeout    Callback is invoked if a timeout has elapsed even if no new frame is ready.
        /// \return CInuError    Error code, InDev::eOK if operation successfully completed.
        virtual CInuError Register(CallbackFunction iCallback, unsigned int iTimeout = FPS_BASED_TIMEOUT) = 0;


    };
}

#endif  //__SLAMSTREAM_H__
