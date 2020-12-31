/*
 * File - PointCloudStream.h
 *
 * This file is part of the Inuitive SDK
 *
 * Copyright (C) 2019 All rights reserved to Inuitive
 *
 */

#ifndef _POINT_CLOUD_STREAM_H__
#define _POINT_CLOUD_STREAM_H__

#include "BasePointCloudStream.h"
#include "PointCloudFrame.h"

namespace InuDev {
////////////////////////////////////////////////////////////////////////
/// \brief   Interface for PointCloud service.
///
/// Role: Controls PointCloud images streaming service and provides PointCloud
/// frames.
///
/// Responsibilities:
///      1. Derives CBasePointCloudStream interface
///      2. Knows how to acquire one PointCloud frame (pull)
///      3. Knows how to provide a continuous stream of PointCloud image frames
///      (push)
///
////////////////////////////////////////////////////////////////////////
class CPointCloudStream : public CBasePointCloudStream {
   public:
    /// \brief    Service initialization.
    ///
    /// Shall be invoked once before starting frames acquisition.
    /// \brief    Service initialization.
    ///
    /// Shall be invoked once before starting frames acquisition.
    /// \param[in] ERegistationType   with/without registration and registration
    /// method. \param[in] iWithConfidence    Acquire PointCloud frames
    /// with/without confidence. \param[in] iChannel             channel id used
    /// for for color registration.
    ///             It is needed only in the case of eRGBRegistation in other
    ///             cases = -1. In the case of eRGBRegistation - 0 - default RGB
    ///             channel.
    /// \return InuDev::CInuError    Error code, InDev::eOK if operation
    /// successfully completed.
    virtual CInuError Init(ERegistationType type = eNoneRegistation,
                           bool iWithConfidence = false,
                           uint32_t iChannel = DEFAULT_CHANNEL_ID,
                           unsigned int iPostProcessingMode = 0) = 0;

    /// \brief    CPointCloudFrame Callback function.
    ///
    /// This function is invoked any time a CPointCloudFrame frame is ready, or
    /// if an error occurs. The parameters of this function are: Caller stream
    /// object, received CPointCloudFrame frames and result code.
    typedef std::function<void(std::shared_ptr<CPointCloudStream>,
                               std::shared_ptr<const CPointCloudFrame>,
                               CInuError)>
        CallbackFunction;

    /// \brief    Retrieves new CPointCloudFrame frame (pull)
    ///
    /// This method returns when a new frame is ready (blocking) or if an input
    /// timeout has elapsed. It shall be called only after a Start() was is
    /// invoked but before any invocation of a Stop() is invoked. \param[out]
    /// oFrame    The returned PointCloud frame . \param[in] iTimeout Function
    /// is returned if timeout has elapsed even if no new frame is ready.
    /// \return CInuError    Error code, InDev::eOK if operation successfully
    /// completed.
    virtual CInuError GetFrame(std::shared_ptr<const CPointCloudFrame>& oFrame,
                               unsigned int iTimeout = FPS_BASED_TIMEOUT) = 0;

    /// \brief    Registration/Unregistration for receiving stream of
    /// CPointCloudFrame frames (push)
    ///
    /// The provided callback function is called when a new frame is ready
    /// (non-blocking). It shall be called only after a Start() was is invoked
    /// but before any invocation of a Stop() is invoked. \param[in] iCallback
    /// Callback function which is invoked when a new frame is ready.
    ///                         Send nullptr to unregister for receiving frames.
    /// \param[in] iTimeout    Callback is invoked if a timeout has elapsed even
    /// if no new frame is ready. \return CInuError    Error code, InDev::eOK if
    /// operation successfully completed.
    virtual CInuError Register(CallbackFunction iCallback,
                               unsigned int iTimeout = FPS_BASED_TIMEOUT) = 0;
};
}  // namespace InuDev

#endif  //_POINT_CLOUD_STREAM_H__
