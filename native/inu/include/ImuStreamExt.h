/*
* File - ImuStreamExt.h
*
* This file is part of the Inuitive SDK
*
* Copyright (C) 2014 All rights reserved to Inuitive
*
*/

#ifndef __IMUSTREAMEXT_H__
#define __IMUSTREAMEXT_H__

#include "ImuStream.h"


namespace InuDev
{
    class CImuStreamExt : public CImuStream
    {
    public:

        CImuStreamExt() : CImuStream () { }

        virtual ~CImuStreamExt() {}

        /// \brief    Set IMU 'OnTheFly' window size
        /// \param[out] oWindowSize    returns the window size - 0 is disabled.
        /// \return CInuError    Error code, InDev::eOK if operation successfully completed.
        virtual CInuError OnTheFlyCompensation(bool iActivate) = 0;

    protected:


    private:

    };
}
#endif // __IMUSTREAMEXT_H__