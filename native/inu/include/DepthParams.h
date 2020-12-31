/*
* File - DepthParams.h
*
* This file is part of the Inuitive SDK
*
* Copyright (C) 2014 All rights reserved to Inuitive
*
*/

#ifndef __DEPTH_PARAMS_H__
#define __DEPTH_PARAMS_H__

namespace InuDev
{
	///////////////////////////////////////////////////////////////////////
	/// \brief    All different Post Preprocessing algorithms
	///			  That's a bitwise mask so multiple modes are allowed
	///////////////////////////////////////////////////////////////////////
	enum EPostProcessingMode
	{
		eDisabled = 0,          ///<  No post processing algorithm is executed (best runtime)
		eBlob = 8,				///<  NU4000 Blob
        eForceBlobInHost = 128,   ///<  Force the use of Blob filter that is implemented in host
	};


    struct CDisparityParams
    {
        ///<  All pixels in the Region 1 area with lower conf value should be set to invalid, should be used in Host disparity2depth algorithm.
        uint8_t     ConfidenceThresholdRegion1;
        ///< All pixels in the Region 2 area with lower conf value should be set to invalid, should be used in Host disparity2depth algorithm.  If  ConfidneceStartRegion2==ConfidneceEndRegion2==0 than this values will not be used
        uint8_t     ConfidenceThresholdRegion2;
        ///< Maximum depth in mm, depth above this value will be invalidated
        uint32_t    MaxDistance;

        ///< Depth scaling factor, 1.0f is to get depth in mm (default).
        float DepthScale;

        ///< Point cloud scaling factor, 0.001f is to get point cloud in meters (default).
        float PointCloudScale;

        CDisparityParams()
            : ConfidenceThresholdRegion1(0)
            , ConfidenceThresholdRegion2(0)
            , MaxDistance(0xFFFFFFFF)
            , DepthScale(1.0f)
            , PointCloudScale(0.001f)
        {}
    };

	class CDepthProperties
	{
	public:
		virtual ~CDepthProperties() {}

		/// \brief SetDisparityParams()
		///
		/// Detailed description:        Send the DisparityParams to Diatarity2Depth filter. By default is set from InuServiceParams.xml
		/// \param[in] params            The new Disparity Params
		/// \return InuDev::CInuError    Return error if user sets the invalid value or error throw during dispatity maps calculation
		///                              otherwise, it is ok.
		virtual CInuError   SetDisparityParams(const CDisparityParams& params) = 0;
		/// \brief GetPointCloudRatio()
		///
		/// Detailed description:        Get current DisparityParams.
		/// \param[out] params           The current DisparityParams.
		/// \return InuDev::CInuError    Return error if current DisparityParams are invalid,
		///                              otherwise, it is ok.
		virtual CInuError   GetDisparityParams(CDisparityParams& params) const = 0;

	protected:

		CDepthProperties() {};
	};
}

#endif // __DEPTH_PARAMS_H__

