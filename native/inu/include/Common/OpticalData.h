#ifndef __OPTICALDATA_H__
#define __OPTICALDATA_H__

#include "InuDefs.h"

namespace InuDev {
/// \brief   Optical Data
class COpticalData {
   public:
    COpticalData()
        : Version(6),
          Baseline(float(UNDEFINED_BASELINE)),
          WebcamDataValid(false),
          IMUParametersValid(false),
          TimeAlignment(0),
          AccelerometerTemperatureReference(0),
          GyroscopeTemperatureReference(0) {
        /*
          memset(&LensDistortionsRealLeft, 0, sizeof(LensDistortionsRealLeft));
          memset(&LensDistortionsRealRight, 0,
          sizeof(LensDistortionsRealRight)); memset(&LensDistortionsRealWebcam,
          0, sizeof(LensDistortionsRealWebcam));
          memset(&AccelerometerMisalignment, 0,
          sizeof(AccelerometerMisalignment)); memset(&AccelerometerScale, 0,
          sizeof(AccelerometerScale)); memset(&AccelerometerBias, 0,
          sizeof(AccelerometerBias)); memset(&AccelerometerNoise, 0,
          sizeof(AccelerometerNoise)); memset(&GyroscopeMisalignment, 0,
          sizeof(GyroscopeMisalignment)); memset(&GyroscopeScale, 0,
          sizeof(GyroscopeScale)); memset(&GyroscopeBias, 0,
          sizeof(GyroscopeBias)); memset(&GyroscopeNoise, 0,
          sizeof(GyroscopeNoise)); memset(&AccelerometerTemperatureSlope, 0,
          sizeof(AccelerometerTemperatureSlope));
          memset(&GyroscopeTemperatureSlope, 0,
          sizeof(GyroscopeTemperatureSlope)); memset(&TCam2IMU, 0,
          sizeof(TCam2IMU));
          */
    }

    virtual ~COpticalData() {}

    // Structure version (for backward compatibility)
    uint32_t Version;

    /// \brief StructuralParameters:

    /// \brief Translation of the real left camera relates to the unit reference
    /// system (right camera reference system) <X,Y,Z> (Extrinsic parameter)
    CPoint3D TranslationRealLeft;

    /// \brief Translation of the real right camera relates to the unit
    /// reference system <X, Y, Z>(Extrinsic parameter)
    CPoint3D TranslationRealRight;

    /// \brief Translation of the Webcam camera relates to the unit reference
    /// system <X,Y,Z> (Extrinsic parameter)
    CPoint3D TranslationRealWebcam;

    /// \brief Translation of the Fisheye camera relates to the unit reference
    /// system <X,Y,Z> (Extrinsic parameter)
    CPoint3D TranslationRealFisheyeLeft;
    CPoint3D TranslationRealFisheyeRight;

    /// \brief Rotation (encoded to the 3 Rodrigues rotation  parameters) of the
    /// real left camera relates to the unit reference system (Extrinsic
    /// parameter)
    CPoint3D RotationRealLeft;

    /// \brief Rotation (encoded to the 3 Rodrigues rotation  parameters) of the
    /// real right camera relates to the unit reference system (Extrinsic
    /// parameter)
    CPoint3D RotationRealRight;

    /// \brief Rotation (encoded to the 3 Rodrigues rotation  parameters) of the
    /// real webcam camera relates to the unit reference system (Extrinsic
    /// parameter)
    CPoint3D RotationRealWebcam;

    /// \brief Rotation (encoded to the 3 Rodrigues rotation  parameters) of the
    /// real Fisheye camera relates to the unit reference system (Extrinsic
    /// parameter)
    CPoint3D RotationRealFisheyeLeft;
    CPoint3D RotationRealFisheyeRight;

    /// \brief Translation of the rectified left camera relates to the unit
    /// reference system <X,Y,Z> (Extrinsic parameter)
    CPoint3D TranslationRectifiedLeft;

    /// \brief Translation of the rectified right camera relates to the unit
    /// reference system <X,Y,Z> (Extrinsic parameter)
    CPoint3D TranslationRectifiedRight;

    /// \brief Rotation (encoded to the 3 Rodrigues rotation  parameters) of the
    /// rectified left camera relates to the unit reference system (Extrinsic
    /// parameter)
    CPoint3D RotationRectifiedLeft;

    /// \brief Rotation (encoded to the 3 Rodrigues rotation  parameters) of the
    /// rectified right camera relates to unit reference system (Extrinsic
    /// parameter)
    CPoint3D RotationRectifiedRight;

    /// \brief OpticalParameters :

    /// \brief Image resolution according to Caltech(Bypass resolution)
    CPoint2D SensorDimensions;

    /// \brief Focal Length <X,Y> of the real left camera (Intrinsic parameter)
    CPoint2D FocalLengthRealLeft;

    /// \brief Focal Length <X,Y> of the real right camera (Intrinsic parameter)
    CPoint2D FocalLengthRealRight;

    /// \brief Focal Length <X,Y> of the real webcam camera (Intrinsic
    /// parameter)
    CPoint2D FocalLengthRealWebcam;

    /// \brief Focal Length <X,Y> of the real Fisheye camera (Intrinsic
    /// parameter)
    CPoint2D FocalLengthRealFisheyeLeft;
    CPoint2D FocalLengthRealFisheyeRight;

    /// \brief Optical center <X,Y> of the real left camera (Intrinsic
    /// parameter)
    CPoint2D OpticalCenterRealLeft;

    /// \brief Optical center <X,Y> of the real right camera (Intrinsic
    /// parameter)
    CPoint2D OpticalCenterRealRight;

    /// \brief Optical center <X,Y> of the Webcam sensor  (Intrinsic parameter)
    CPoint2D OpticalCenterRealWebcam;

    /// \brief Optical center <X,Y> of the Fisheye sensor  (Intrinsic parameter)
    CPoint2D OpticalCenterRealFisheyeLeft;
    CPoint2D OpticalCenterRealFisheyeRight;

    static const int DISTORTION_PARAMETERS_SIZE = 5;
    static const int DISTORTION_PARAMETERS_SIZE_FISHEYE = 4;

    /// \brief Real left camera Distortion constants <K1,K2,K3,K4,K5> (Intrinsic
    /// parameter)
    float LensDistortionsRealLeft[DISTORTION_PARAMETERS_SIZE];

    /// \brief Real right camera Distortion constants <K1,K2,K3,K4,K5>
    /// (Intrinsic parameter)
    float LensDistortionsRealRight[DISTORTION_PARAMETERS_SIZE];

    /// \brief Webcam Distortion constants <K1,K2,K3,K4,K5> (Intrinsic
    /// parameter)
    float LensDistortionsRealWebcam[DISTORTION_PARAMETERS_SIZE];

    /// \brief Fisheye Distortion constants <K1,K2,K3,K4> (Intrinsic parameter)
    float LensDistortionsRealFisheyeLeft[DISTORTION_PARAMETERS_SIZE_FISHEYE];
    float LensDistortionsRealFisheyeRight[DISTORTION_PARAMETERS_SIZE_FISHEYE];

    /// \brief RectifiedParameters :

    /// \brief Focal Length of the left sensor (Intrinsic parameter)
    CPoint2D FocalLengthBaseLeft;

    /// \brief Focal Length of the Right sensor (Intrinsic parameter)
    CPoint2D FocalLengthBaseRight;

    /// \brief Optical center <X,Y> of the left sensor (Intrinsic parameter)
    CPoint2D OpticalCenterBaseLeft;

    /// \brief Optical center <X,Y> of the right sensor (Intrinsic parameter)
    CPoint2D OpticalCenterBaseRight;

    /// \brief Internal use for fine tuning
    CPoint2D TranslateUV;

    /// \brief General Parameters :
    CPoint2D Squint;

    /// \brief	Physical distance between stereo cameras (left and right)
    float Baseline;

    /// \brief	Baseline is not defined in old versions of COpticalData
    static const int UNDEFINED_BASELINE = -1;

    /// \brief    true if and only if Webcam parameters are valid
    bool WebcamDataValid;  //  from version 3

    /// \brief    true if and only if Fisheye parameters are valid
    bool FisheyeDataValidLeft;
    bool FisheyeDataValidRight;

    /// \brief    true if and only if IMU parameters are valid
    bool IMUParametersValid;

    /// \brief    IMU Intrinsic parameters
    /////////////////////////////////////
    float AccelerometerMisalignment[9];
    float AccelerometerScale[9];
    float AccelerometerBias[3];
    float AccelerometerNoise[3];

    float GyroscopeMisalignment[9];
    float GyroscopeScale[9];
    float GyroscopeBias[3];
    float GyroscopeNoise[3];

    float TimeAlignment;

    float AccelerometerTemperatureSlope[3];
    float AccelerometerTemperatureReference;

    float GyroscopeTemperatureSlope[3];
    float GyroscopeTemperatureReference;

    /// \brief    IMU Extrinsic parameters
    /////////////////////////////////////
    float TCam2IMU[16];
};
}  // namespace InuDev

#endif
