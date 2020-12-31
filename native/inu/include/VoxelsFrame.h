/*
* File - VoxelsFrame.h
*
* This file is part of the Inuitive SDK
*
* Copyright (C) 2020 All rights reserved to Inuitive
*
*/

#ifndef _VOXELS_FRAME_H__
#define _VOXELS_FRAME_H__

#include "StreamsExport.h"
#include "GeneralFrame.h"
#include "InuSensor.h"

#include <map>

namespace pcl
{
	struct PCLPointCloud2;
}

namespace InuDev
{
	////////////////////////////////////////////////////////////////////////
	/// \brief    Represents  an VoxelsFrame Frame that is  provided  by InuDev Voxels stream
	///
	/// Responsibilities:
	///      1. Frame attributes: Format, Confidence,  NumOfPoints  and number of bytes per pixel.
	///      2. Knows how to manage the  buffer.
	///
	////////////////////////////////////////////////////////////////////////
	class CVoxelsFrame : public CGeneralFrame
	{

	public:

		virtual ~CVoxelsFrame() {}

		std::shared_ptr<const pcl::PCLPointCloud2> GetVoxels() const;

		/// \brief    get Exposure parameters to update
		const std::map<uint16_t, CExposureParams>&   GetSensorsExposureParams() const ;

	protected:
		/// \brief    Constructs empty (zero size) frame
		INUSTREAMS_API CVoxelsFrame();

		/// \brief    Constructor.
		/// \param[in] uint32_t iBuffSize
		INUSTREAMS_API CVoxelsFrame(uint32_t iBuffSize);

		/// \brief    Holds the ET, digital and analog gain for one stream
		std::map<uint16_t, CExposureParams> mExposureParams;

		std::shared_ptr<pcl::PCLPointCloud2>  mVoxels;
	};

	inline std::shared_ptr<const pcl::PCLPointCloud2> CVoxelsFrame::GetVoxels() const
	{
		return mVoxels;
	}

	inline const std::map<uint16_t, CExposureParams>&   CVoxelsFrame::GetSensorsExposureParams() const
	{
		return mExposureParams;
	}
}

#endif //_VOXELS_FRAME_H__
