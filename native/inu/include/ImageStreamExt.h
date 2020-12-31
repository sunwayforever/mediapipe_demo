/*
* File - ImageStreamExt.h
*
* This file is part of the Inuitive SDK
*
* Copyright (C) 2014 All rights reserved to Inuitive
*
*/

#ifndef __IMAGESTREAMEXT_H__
#define __IMAGESTREAMEXT_H__

#include "ImageStream.h"
#include "DepthParams.h"

namespace InuDev
{
    ////////////////////////////////////////////////////////////////////////
    /// \brief Image stream extension class
    ///
    ///
    /// Role: Adds extended features to the ImageStream interface
    ///       Intended for internal use
    ///
    ///
    /// Responsibilities:
    ///      1. Add extended API interface
    ///
    /// Comments:
    ////////////////////////////////////////////////////////////////////////
    class CImageStreamExt : public CImageStream, public CDepthProperties
    {
    public:
        virtual ~CImageStreamExt() {}

	protected:
		CImageStreamExt() {}
    };
}

#endif // __IMAGESTREAMEXT_H__

