#ifndef __ALGODEPTHOPTICALDATA__
#define __ALGODEPTHOPTICALDATA__

#include <utility>

#include "CalibrationData.h"
#include "InuDefs.h"

namespace cv {
class Mat;
}

namespace InuCommon {
////////////////////////////////////////////////////////////////////////
/// \brief    Enhanced Optical Data which is used for Disparity2Depth
/// calculation
///
/// Responsibilities:
///      1. Disparity image data
///      2. Knows how to calculate Optical Factor and Disparity Offset
////////////////////////////////////////////////////////////////////////
class CDepthOpticalData : public InuDev::CCalibrationData {
   public:
    /// \brief def Constructor.
    /// \param[in] iOpticalData    Optical Data object.
    /// \param[in] iStereoSensors    Stereo sensors ids: first is depth match
    /// (right)
    CDepthOpticalData()
        : mDownScaleFactor(1.0f),
          mHorizPadding(0),
          mVerticalPadding(0),
          mOpticalFactor(0),
          mDisparityOffset(0),
          mStereoSensors(std::make_pair(0, 1)) {}

    /// \brief    Constructor.
    /// \param[in] iOpticalData    Optical Data object.
    /// \param[in] iStereoSensors    Stereo sensors ids: first is depth match
    /// (right) \param[in] iBaseline   Distance between stereo sensors
    /// \param[in,out] iDownScaleFactor
    /// \param[in,out] iHorizPadding
    /// \param[in,out] iVerticalPadding
    CDepthOpticalData(const InuDev::CCalibrationData& iOpticalData,
                      const std::pair<int, int>& iStereoSensors,
                      float iBaseline, float iDownScaleFactor = 1.0f,
                      short iHorizPadding = 0, short iVerticalPadding = 0)
        : InuDev::CCalibrationData(iOpticalData),
          mDownScaleFactor(iDownScaleFactor),
          mHorizPadding(iHorizPadding),
          mVerticalPadding(iVerticalPadding),
          mStereoSensors(iStereoSensors) {
        mOpticalFactor = float(
            2 *
            Sensors[iStereoSensors.second]
                .VirtualCamera.Intrinsic.FocalLength[0] *
            iBaseline / mDownScaleFactor);  // factor 2 is because disparity is
                                            // on half pixel units
        mDisparityOffset =
            float(2 *
                      (Sensors[iStereoSensors.first]
                           .VirtualCamera.Intrinsic.OpticalCenter[0] -
                       Sensors[iStereoSensors.second]
                           .VirtualCamera.Intrinsic.OpticalCenter[0]) /
                      mDownScaleFactor -
                  2 * mHorizPadding);
    }

    InuDev::CPoint2D GetFocalLengthRealRight() const {
        return InuDev::CPoint2D(
            float(Sensors.at(mStereoSensors.first)
                      .RealCamera.Intrinsic.FocalLength[0]),
            float(Sensors.at(mStereoSensors.first)
                      .RealCamera.Intrinsic.FocalLength[1]));
    }

    /// \brief  Down scale factor - for Binning set 2, for upscale X2 set 0.5
    float mDownScaleFactor;

    ///  \brief  Padding (added empty Pixels) on the left of the left image and
    ///  on the right of the right image. For crop use negative values
    short mHorizPadding;
    short mVerticalPadding;

    ///  \brief   Used for Disparity to Depth calculation
    float mOpticalFactor;
    float mDisparityOffset;

    std::pair<int, int> mStereoSensors;
};
}  // namespace InuCommon

#endif
