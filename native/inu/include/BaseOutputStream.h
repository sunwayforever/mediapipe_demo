/*
* File - BaseOutputStream.h
*
* This file is part of the Inuitive SDK
*
* Copyright (C) 2019 All rights reserved to Inuitive
*
*/

#ifndef __BASE_OUTPUT_STREAM_H__
#define __BASE_OUTPUT_STREAM_H__

#include "BaseStream.h"

namespace InuDev
{

    ////////////////////////////////////////////////////////////////////////
    /// \brief   Common interface for all InuDev NUI output streams.
    ///
    /// Role: Base class for all output streams classes.
    /// It defines an interface that should be implemented by all derived classes.
    ///
    /// Responsibilities:
    ///      1. Knows how to control the service
    ///      2. Controls the frame acquisition time out
    ////////////////////////////////////////////////////////////////////////
    class CBaseOutputStream : public CBaseStream
    {
    public:

        /// \brief    Default timeout for frames acquisition.
        ///
        /// Default is FPS based, which means that timeout is derived from the configured number of frames per second
        static const unsigned int FPS_BASED_TIMEOUT=0;

        static const uint64_t RECORD_INFINITE = uint64_t(-1);

        virtual ~CBaseOutputStream() {}

        /// \brief    brief Set the default timeout for frames acquisition.
        ///
        /// If this timeout is elapsed then the frame acquisition method returns with timeout error
        /// \param[in] iTimeout    requested timeout.
        /// \return void    description.
        virtual void SetFrameTimeout(unsigned int iTimeout) = 0;

        /// \brief Record NU streams
        /// \param[in]  iDestinationDirectory Destination directory for recording output. Send empty string to stop recording.
        /// \param[in]  iTemplateName string which will be concatenate to output file name.
        /// \param[in]  iDuration recording time in ms.
        /// \return CInuError eOK indicates that request was successfully processed but it doesn't indicate that recording is completed
        virtual CInuError  Record(const std::string& iDestinationDirectory,
            const std::string& iTemplateName = std::string(),
            uint64_t iDuration = CBaseOutputStream::RECORD_INFINITE) const = 0;

        /// \brief Record NU streams
        /// \param[in]  iDestinationDirectory Destination directory for recording output. Send empty string to stop recording.
        /// \param[in]  iTemplateName string which will be concatenate to output file name.
        /// \param[in]  iFileNameIndex string which will be concatenate to output file name. If no index is provided then frame index will be used.
        /// \return CInuError eOK indicates that request was successfully processed but it doesn't indicate that recording is completed
        virtual CInuError  Snapshot(const std::string& iDestinationDirectory,
            const std::string& iTemplateName = std::string(),
            uint64_t iFileNameIndex = CBaseOutputStream::RECORD_INFINITE) const = 0;
    };
}

#endif // __BASE_OUTPUT_STREAM_H__
