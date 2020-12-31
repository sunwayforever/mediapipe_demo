/*
* File - HistogramStream.h
*
* This file is part of the Inuitive SDK
*
* Copyright (C) 2014 All rights reserved to Inuitive
*
*/

#ifndef __HISTOGRAMSTREAM_H__
#define __HISTOGRAMSTREAM_H__

#include "BaseOutputStream.h"
#include "BaseFrame.h"

#include <functional>
#include <vector>

namespace InuDev
{
    /// \brief    Represents collection of Histogram data that is provided by Histogram
    class CHistogramFrame : public CBaseFrame
    {
	public:

		/// \brief    Constructs empty (zero size) buffer
		INUSTREAMS_API CHistogramFrame() {}

		struct histogramROI
		{
			CPoint<2, uint16_t> topLeft;
			CPoint<2, uint16_t> bottomRight;
			std::vector<uint32_t> histogram;
			uint32_t	accumulator = 0;
		};

		const std::vector<histogramROI>& GetHistograms() const { return mHistograms;  }

	protected:
		std::vector<histogramROI> mHistograms;
    };

    ////////////////////////////////////////////////////////////////////////
    /// \brief   Interface for Histogram service.
    ///
    /// Role: Controls Histogram streaming service and provides general or Histogram frames.
    ///       Histogram frames are provided only if the connected device supports Histogram HW components.
    ///       The caller application should be familiar with provided frames and should know how to interpret them.
    ///
    /// Responsibilities:
    ///      1. Derives CBaseOutputStream interface
    ///      2. Knows how to acquire one general frame (pull)
    ///      3. Knows how to provide a continuous stream of general frames (push)
    ///
    ////////////////////////////////////////////////////////////////////////
    class CHistogramStream : public CBaseOutputStream
    {
    public:

         /// \brief    Prototype of callback function which is used by the Register method (Histogram Frames).
        ///
        /// This function is invoked any time a frame is ready, or if an error occurs. The parameters of this function are:
        /// Caller stream object, received IMU frame and result code.
        typedef std::function<void(std::shared_ptr<CHistogramStream> , std::shared_ptr<const CHistogramFrame>,  CInuError)>     CallbackFunction;



        virtual ~CHistogramStream() {}

        /// \brief    Retrieves CHistogramStream frame (pull)
        ///
        /// This method returns when a new CHistogramStream is ready (blocking) or if an input timeout has elapsed.
        /// It shall be called only after a Start() was is invoked and but before any invocation of a Stop() is invoked.
        /// \param[out] oImuFrame    Array of CImuData sensor values.
        /// \param[in] iTimeout    Function is returned if timeout has elapsed even if no new frame is ready.
        /// \return CInuError    Error code, InDev::eOK if operation successfully completed.
        virtual CInuError GetFrame(std::shared_ptr<const CHistogramFrame>& oHistogramFrame, unsigned int iTimeout = FPS_BASED_TIMEOUT) = 0;

        /// \brief    Registration/Unregistration for receiving stream of general frames (push)
        ///
        /// The provided callback function is called when a new frame is ready (non-blocking).
        /// It shall be called only after a Start() was is invoked and but before any invocation of a Stop() is invoked.
        /// \param[in] iCallback    Callback function which is invoked when a new frame is ready.
        ///                         Send nullptr to unregister for receiving frames.
        /// \param[in] iTimeout    Callback is invoked if a timeout has elapsed even if no new frame is ready.
        /// \return CInuError    Error code, InDev::eOK if operation successfully completed.
        virtual CInuError Register(CallbackFunction iCallback, unsigned int iTimeout = FPS_BASED_TIMEOUT) = 0;
    };
}

#endif
