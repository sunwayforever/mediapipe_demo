/*
* File - VoxeslStream.h
*
* This file is part of the Inuitive SDK
*
* Copyright (C) 2020 All rights reserved to Inuitive
*
*/

#ifndef _VOXEL_STREAM_H__
#define _VOXEL_STREAM_H__

#include "BasePointCloudStream.h"

namespace InuDev
{
	class CVoxelsFrame;

    ////////////////////////////////////////////////////////////////////////
    /// \brief   Interface for Voxels service.
    ///
    /// Role: Controls Voxels frames streaming service and provides Voxels frames.
    ///
    /// Responsibilities:
    ///      1. Derives CBasePointCloudStream interface
    ///      2. Knows how to acquire one  Voxels frame (pull)
    ///      3. Knows how to provide a continuous stream of Voxels image frames (push)
    ///
    ////////////////////////////////////////////////////////////////////////

	static const float DEFAULT_LEAF_SIZE = (float)0.02;//meter

    class CVoxelsStream : public CBasePointCloudStream
    {
	public:

		struct Params
		{
			Params(
				float iLeafsizeX = DEFAULT_LEAF_SIZE,
				float iLeafsizeY = DEFAULT_LEAF_SIZE,
				float iLeafsizeZ = DEFAULT_LEAF_SIZE)
				: LeafsizeX(iLeafsizeX)
				, LeafsizeY(iLeafsizeY)
				, LeafsizeZ(iLeafsizeZ)
			{

			}
			/// \brief
			///
			//voxels grid leaf sizes.
			float LeafsizeX;			//voxel grid leaf size x
			float LeafsizeY;			//voxel grid leaf size y
			float LeafsizeZ;			//voxel grid leaf size z
		};

    public:
		using CBaseStream::Init;

		/// \brief    Service initialization.
		///
		/// Shall be invoked once before starting frames acquisition.
		/// \brief    Service initialization.
		///
		/// Shall be invoked once before starting frames acquisition.
		/// \param[in] Params      voxel grid leaf sizes.
		/// \param[in]  iChannel   depth   channel id.
		/// \return InuDev::CInuError    Error code, InDev::eOK if operation successfully completed.
		virtual CInuError   Init(const CVoxelsStream::Params& iVoxelsParams, uint32_t iChannel = DEFAULT_CHANNEL_ID, unsigned int iPostProcessingMode = 0) = 0;

		/// \brief    CVoxelsFrame Callback function.
        ///
        /// This function is invoked any time a CVoxelsFrame frame is ready, or if an error occurs.
        /// The parameters of this function are: Caller stream object, received CVoxelsFrame frames and result code.
        typedef std::function<void(std::shared_ptr<CVoxelsStream>, std::shared_ptr<const CVoxelsFrame>, CInuError)> CallbackFunction;

        /// \brief    Retrieves new CVoxelsFrame frame (pull)
        ///
        /// This method returns when a new frame is ready (blocking) or if an input timeout has elapsed.
        /// It shall be called only after a Start() was is invoked but before any invocation of a Stop() is invoked.
        /// \param[out] oFrame    The returned PointCloud frame .
        /// \param[in] iTimeout    Function is returned if timeout has elapsed even if no new frame is ready.
        /// \return CInuError    Error code, InDev::eOK if operation successfully completed.
        virtual CInuError  GetFrame(std::shared_ptr<const CVoxelsFrame>& oFrame, unsigned int iTimeout = FPS_BASED_TIMEOUT) = 0;

        /// \brief    Registration/Unregistration for receiving stream of CPointCloudFrame frames (push)
        ///
        /// The provided callback function is called when a new frame is ready (non-blocking).
        /// It shall be called only after a Start() was is invoked but before any invocation of a Stop() is invoked.
        /// \param[in] iCallback    Callback function which is invoked when a new frame is ready.
        ///                         Send nullptr to unregister for receiving frames.
        /// \param[in] iTimeout    Callback is invoked if a timeout has elapsed even if no new frame is ready.
        /// \return CInuError    Error code, InDev::eOK if operation successfully completed.
        virtual CInuError Register(CallbackFunction iCallback, unsigned int iTimeout = FPS_BASED_TIMEOUT) = 0;

		/// \brief    SetVoxelsParams
		///
		/// \param	[in]	iParams	the VoxelsParams struct
		virtual CInuError  SetVoxelsParams(const CVoxelsStream::Params& iParams) = 0;

		/// \brief    GetVoxelsParams
		///
		/// \param	[out]	oParams	the VoxelsParams struct
		virtual CInuError  GetVoxelsParams(CVoxelsStream::Params& oParams) = 0;
	};
}

#endif //_VOXEL_STREAM_H__
