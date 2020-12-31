/*
 * File - InputStream.h
 *
 * This file is part of the Inuitive SDK
 *
 * Copyright (C) 2014 All rights reserved to Inuitive
 *
 */

#ifndef __INJECTIONREAM_H__
#define __INJECTIONREAM_H__

#include "BaseInputStream.h"
#include "ImageFrame.h"
#include "InuError.h"

namespace InuDev {

////////////////////////////////////////////////////////////////////////
/// \brief   Interface for Injection Input  service.
///
/// Role: Controls input injection streaming frames service
///
/// Responsibilities:
///      1. Knows how to inject input frame buffer
///
////////////////////////////////////////////////////////////////////////
class CInjectionStream : public CBaseInputStream {
   public:
    static const unsigned int DEFAULT_BUFF_SIZE = 36000000;

    CInjectionStream() {}
    virtual ~CInjectionStream() {}

    using CBaseStream::Init;

    /// \brief    Init - Injection stream Initialization.
    /// \param[in] iData    Frame  Data buffer size used in memory pool build
    virtual CInuError Init(unsigned int iBufferSize) = 0;

    // One Step Injection - user need to fill his frame and call to :
    virtual CInuError InjectFrame(std::shared_ptr<CImageFrame> iFrame) = 0;

    // Two steps injection:
    // 1st AllocateFrame - user will receive the allocated (ready for using)
    // frame
    virtual CInuError AllocateFrame(
        std::shared_ptr<CImageFrame>& iFrame,
        unsigned int iBufferSize = DEFAULT_BUFF_SIZE) = 0;
    // 2nd InjectAllocatedFrame - and set it by call :
    virtual CInuError InjectAllocatedFrame(
        std::shared_ptr<CImageFrame> iFrame) = 0;
};
}  // namespace InuDev

#endif  //  __INJECTIONREAM_H__
