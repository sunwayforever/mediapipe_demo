/*
* File - BaseStream.h
*
* This file is part of the Inuitive SDK
*
* Copyright (C) 2014 All rights reserved to Inuitive
*
*/

#ifndef __BASESTREAM_H__
#define __BASESTREAM_H__

#include "BaseFrame.h"
#include <memory>

namespace InuDev
{
    /// \brief    Stream's state is always one of EStreamState
    enum EStreamState
    {
        eUninitialized = 0,
        eInitialized,
        eStarted,
        eSensorDisconnected       /// Sensor is disconnected
    };

    ////////////////////////////////////////////////////////////////////////
    /// \brief   Common interface for all InuDev NUI streams.
    ///
    /// Role: Base class for all streams classes. It defines an interface that should be implemented by all derived classes.
    ///
    /// Responsibilities:
    ///      1. Knows how to control the service (Init, Terminate, Start and Stop)
    ///      2. Implements a state machine (more details in Reference Manual)
    ////////////////////////////////////////////////////////////////////////
    class CBaseStream
    {
    public:

        virtual ~CBaseStream() {}

        /// \brief    Service initialization.
        ///
        /// Shall be invoked once before starting frames acquisition.
        /// \return CInuError    Operation status which indicates on success or failure.
        virtual CInuError Init() = 0;

        /// \brief    Service termination.
        ///
        /// Shall be invoked when the service is no longer in use and after frames acquisition has stopped.
        /// \return CInuError    Operation status which indicates success or failure.
        virtual CInuError Terminate() = 0;

        /// \brief    Start acquisition of frames.
        ///
        /// Shall be invoked only after the service is successfully initialized and before any request
        /// for new frame (push or pull).
        /// \param[in] iUserID    Unique ID of the user who should be tracked.
        /// \return CInuError    Operation status.
        virtual CInuError Start(const std::string& iUserID = std::string()) = 0;

        /// \brief    Stop acquisition of frames.
        ///
        /// Shall be invoked after requests for frames are no longer sent and before service termination
        /// (only if Start() was invoked).
        /// \return CInuError    Operation status which indicates success or failure.
        virtual CInuError Stop() = 0;

        /// \brief    Stream state information.
        /// \return EStreamState    Stream's current state.
        virtual EStreamState State() const = 0;

        /// \brief    Final stream channel ID as recieved from InuService.
        /// \return CInuError    returns error if the stream is not initialized 
		/// or if the channel is invalid, Otherwise returns eOK.
        virtual CInuError GetChannel(uint32_t& oChannelID) = 0;

    };
}

#endif // __BASESTREAM_H__
