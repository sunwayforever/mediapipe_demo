/*
* File - UserDefinedStream.h
*
* This file is part of the Inuitive SDK
*
* Copyright (C) 2014 All rights reserved to Inuitive  
*
*/


#ifndef __UserDefinedStream_H__
#define __UserDefinedStream_H__

#include "BaseOutputStream.h"
#include "GeneralFrame.h"

#include <functional>

namespace InuDev 
{

    ////////////////////////////////////////////////////////////////////////
    /// \brief   Interface for General Purpose service.
    /// 
    /// Role: Controls general purpose streaming service and provides general frames. 
    ///       General Purpose streaming service executes user defined code on CEVA and provides
    ///       general frames. The caller application should know how to interpret this buffer.
    ///
    /// Responsibilities: 
    ///      1. Derives CBaseStream interface
    ///      2. Knows how to acquire one general frame (pull)
    ///      3. Knows how to provide a continuous stream of general frames (push)
    /// 
    //////////////////////////////////////////////////////////////////////// 
    class CUserDefinedStream : public CBaseOutputStream
    {
    public:

        /// \brief    Prototype of callback function which is used by the Register method.
        ///
        /// This function is invoked any time a frame is ready, or if an error occurs. The parameters of this function are:
        /// Caller stream object, received general frame and result code.
        typedef std::function<void(std::shared_ptr<CUserDefinedStream> , std::shared_ptr<const CGeneralFrame>,  CInuError)> CallbackFunction;

        virtual ~CUserDefinedStream() {}

        /// \brief    Write buffer to General Purpose service.
        /// 
        /// This function is used to transfer data to general purpose service which is executed on device.
        /// The caller application should know how to pack the data so it will be successfully interpreted by the service.
        /// \param[in] iBuffer    Transfered data.
        /// \param[in,out] iBufferLen    Transferred buffer length.
        /// \return InuDev::CInuError     Error code, InDev::eOK if operation successfully completed.
        virtual CInuError Write(const unsigned char* iBuffer, unsigned int iBufferLen) = 0;

        /// \brief    Write buffer to General Purpose service.
        /// 
        /// This function is used to "Control" the user defined stream.
        /// The caller application should know how to pack the data so it will be successfully interpreted by the service.
        /// \param[in] iBuffer    Transfered data.
        /// \param[in,out] iBufferLen    Transferred buffer length.
        /// \return InuDev::CInuError     Error code, InDev::eOK if operation successfully completed.
        virtual CInuError Control(const unsigned char* iBuffer, unsigned int iBufferLen) = 0;

        /// \brief    Retrieves new general frame (pull) 
        /// 
        /// This method returns when a new frame is ready (blocking) or if an input timeout has elapsed. 
        /// It shall be called only after a Start() was is invoked but before any invocation of a Stop() is invoked.
        /// \param[out] oUserDefinedFrame    The returned general frame (user defined data).
        /// \param[in] iTimeout    Function is returned if timeout has elapsed even if no new frame is ready.
        /// \return CInuError    Error code, InDev::eOK if operation successfully completed.
        virtual CInuError GetFrame(std::shared_ptr<const CGeneralFrame>& oUserDefinedFrame, unsigned int iTimeout = FPS_BASED_TIMEOUT) = 0;

        /// \brief    Registration/Unregistration for receiving stream of general frames (push) 
        /// 
        /// The provided callback function is called when a new frame is ready (non-blocking). 
        /// It shall be called only after a Start() was is invoked but before any invocation of a Stop() is invoked.
        /// \param[in] iCallback    Callback function which is invoked when a new frame is ready.
        /// \param[in] iTimeout    Callback is invoked if a timeout has elapsed even if no new frame is ready.
        /// \return CInuError    Error code, InDev::eOK if operation successfully completed.
        virtual CInuError Register(CallbackFunction iCallback, unsigned int iTimeout = FPS_BASED_TIMEOUT) = 0;

    };
}

#endif
