/*
 * File - BasePointCloudtream.h
 *
 * This file is part of the Inuitive SDK
 *
 * Copyright (C) 2020 All rights reserved to Inuitive
 *
 */

#ifndef _BASE_POINTCLOUD_STREAM_H__
#define _BASE_POINTCLOUD_STREAM_H__

#include "BaseOutputStream.h"
#include "DepthParams.h"
#include "InuSensor.h"

namespace InuDev {
class CBasePointCloudStream : public CBaseOutputStream,
                              public CDepthProperties {
   public:
    enum ERegistationType { eNoneRegistation, eRGBRegistation };

    /// \brief SetPointCloudFPS()
    ///
    /// Detailed description:        Send the user point cloud FPS. by default
    /// is set to be same as the CPointCloudFrame frame FPS \param[in] iFPS The
    /// new fps \return InuDev::CInuError    Return error if user sets the fps
    /// to be 0 or greater than the PointCloud fps,
    ///                              otherwise, it is ok.
    virtual CInuError SetPointCloudFPS(unsigned int iFPS) = 0;

    /// \brief SetDecimationFactor()
    ///
    /// Detailed description:        Set new DecimationFactor.
    /// \param[in] iVal              DecimationFactor (width,height).
    /// \return InuDev::CInuError    Return error if current DisparityParams are
    /// invalid,
    ///                              otherwise, it is ok.
    virtual CInuError SetDecimationFactor(
        const std::pair<uint32_t, uint32_t>& iVal) = 0;

    /// \brief GetDecimationFactor()
    ///
    /// Detailed description:        Get current DecimationFactor.
    /// \param[out] oVal             The current DecimationFactor
    /// (width,height). \return InuDev::CInuError    Return error if current
    /// DisparityParams are invalid,
    ///                              otherwise, it is ok.
    virtual CInuError GetDecimationFactor(
        std::pair<uint32_t, uint32_t>& oVal) = 0;
};
}  // namespace InuDev

#endif  //_BASE_POINTCLOUD_STREAM_H__
