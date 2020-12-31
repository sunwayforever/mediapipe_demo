/*
* File - BaseInputStream.h
*
* This file is part of the Inuitive SDK
*
* Copyright (C) 2019 All rights reserved to Inuitive
*
*/

#ifndef __BASE_INPUT_STREAM_H__
#define __BASE_INPUT_STREAM_H__

#include "BaseStream.h"
#include "CnnStream.h"

namespace InuDev
{
    ////////////////////////////////////////////////////////////////////////
    /// \brief   Common interface for all InuDev NUI input streams.
    ///
    /// Role: Base class for all streams classes. It defines an interface that should be implemented by all derived classes.
    ///
    /// Responsibilities:
    ///      1. Implements a state machine (more details in Reference Manual)
    ///      2. Controls the write data in
    ////////////////////////////////////////////////////////////////////////
    class CBaseInputStream :public CBaseStream
    {
    public:

        virtual ~CBaseInputStream() {}

        /// \brief    Write buffer to NU service.
        /// \param[in] iBuffer      Pointer to buffer.
        /// \param[in] iBufferLen   Buffer length.
        /// \return InuDev::CInuError    Error code, InDev::eOK if operation successfully completed.
        virtual CInuError WriteData(const unsigned char* iBuffer, unsigned int iBufferLen) = 0;
    };
}

#endif // __BASE_INPUT_STREAM_H__
