/*
 * File - PointCloudFrame.h
 *
 * This file is part of the Inuitive SDK
 *
 * Copyright (C) 2019 All rights reserved to Inuitive
 *
 */

#ifndef __POINT_CLOUD_FRAME_H__
#define __POINT_CLOUD_FRAME_H__

#include "GeneralFrame.h"
#include "InuSensor.h"
#include "StreamsExport.h"

namespace InuDev {
struct CPointCloudFrameParams {
    CPointCloudFrameParams() : channelId(0), frameStart(0), frameSize(0) {}

    unsigned short channelId;
    unsigned short frameStart;
    unsigned short frameSize;
};

////////////////////////////////////////////////////////////////////////
/// \brief    Represents  an PointCloud Frame that is  provided  by InuDev
/// PointCloud stream
///
/// Responsibilities:
///      1. Frame attributes: Format, Confidence,  NumOfPoints  and number of
///      bytes per pixel.
///      2. Knows how to manage the  buffer.
///
////////////////////////////////////////////////////////////////////////
class CPointCloudFrame : public CGeneralFrame {
   public:
    /// \brief    Point Cloud pixel in e3DPoints format
    typedef CPoint3D C3DPixel;
    /// \brief    Point Cloud pixel in e3DPointsConf format
    typedef CPoint4D C3DConfPixel;
    /// \brief    Point Cloud pixel in e3DPointsRGB format
    struct C3DRGBPixel : public CPoint3D {
        byte R;
        byte G;
        byte B;
        byte Alpha;
    };

    /// \brief    Point Cloud pixel in e3DPointsRGBConf format
    struct C3DRGBConfPixel : public CPoint3D {
        byte R;
        byte G;
        byte B;
        byte Alpha;
        byte Confidence;
    };

    /// \brief    Point Cloud Format  formats
    enum EFormat { e3DPoints, e3DPointsConf, e3DPointsRGB, e3DPointsRGBConf };

    /// \brief    Destructor.
    INUSTREAMS_API virtual ~CPointCloudFrame();

    /// \brief    Image width
    unsigned int GetNumOfPoints() const;

    /// \brief     Point Format
    INUSTREAMS_API EFormat GetFormat() const;

    /// \brief    Image Size - number of pixels
    unsigned int BufferSize() const;

    /// \brief    Non mutable pointer to Frame data in e3DPoints format -
    /// C3DPixel
    ///  in the another format case  return nullptr
    INUSTREAMS_API const C3DPixel* Get3DData() const;
    /// \brief    Non mutable pointer to Frame data in e3DPointsConf format -
    /// C3DConfPixel
    ///  in the another format case  return nullptr
    INUSTREAMS_API const C3DConfPixel* Get3DConfData() const;
    /// \brief    Non mutable pointer to Frame data in e3DPointsRGB format -
    /// C3DRGBPixel
    ///  in the another format case  return nullptr
    INUSTREAMS_API const C3DRGBPixel* Get3DRGBData() const;
    /// \brief    Non mutable pointer to Frame data in e3DPointsRGBConf format -
    /// C3DRGBConfPixel
    ///  in the another format case  return nullptr
    INUSTREAMS_API const C3DRGBConfPixel* Get3DRGBConfData() const;

    ///// \brief    Number of bytes that are used to represent each point.
    INUSTREAMS_API unsigned int BytesPerPoint() const;

    /// \brief    Exposure parameters while this image was grabbed
    const std::map<uint16_t, CExposureParams>& GetSensorsExposureParams() const;

    static unsigned int BytesPerFormat(CPointCloudFrame::EFormat format);

   protected:
    /// \brief    Constructs empty (zero size) frame
    INUSTREAMS_API CPointCloudFrame();

    /// \brief    Constructor.
    /// \param[in] iNumOfPoints   Number Of Points
    /// \param[in] iFormat        Image format (which defines  the number of
    /// bytes  per pixel).
    INUSTREAMS_API CPointCloudFrame(unsigned int iNumOfPoints, EFormat iFormat);

    /// \brief     Number Of Points
    unsigned int mNumOfPoints;

    /// \brief     Point Format
    EFormat mFormat;

    /// \brief    get Exposure parameters to update
    std::map<uint16_t, CExposureParams>& GetSensorsExposureParams();

    /// \brief    Holds the ET, digital and analog gain for one stream
    std::map<uint16_t, CExposureParams> mExposureParams;
};

inline CPointCloudFrame::EFormat CPointCloudFrame::GetFormat() const {
    return mFormat;
}

inline const std::map<uint16_t, CExposureParams>&
CPointCloudFrame::GetSensorsExposureParams() const {
    return mExposureParams;
}

inline unsigned int CPointCloudFrame::GetNumOfPoints() const {
    return mNumOfPoints;
}

inline unsigned int CPointCloudFrame::BufferSize() const {
    return GetDataSize();
}

inline unsigned int CPointCloudFrame::BytesPerPoint() const {
    return BytesPerFormat(mFormat);
}

inline const CPointCloudFrame::C3DPixel* CPointCloudFrame::Get3DData() const {
    if (mFormat != e3DPoints) {
        return nullptr;
    }

    return (CPointCloudFrame::C3DPixel*)CGeneralFrame::GetData();
}

inline const CPointCloudFrame::C3DConfPixel* CPointCloudFrame::Get3DConfData()
    const {
    if (mFormat != CPointCloudFrame::e3DPointsConf) {
        return nullptr;
    }

    return (CPointCloudFrame::C3DConfPixel*)CGeneralFrame::GetData();
}

inline const CPointCloudFrame::C3DRGBPixel* CPointCloudFrame::Get3DRGBData()
    const {
    if (mFormat != CPointCloudFrame::e3DPointsRGB) {
        return nullptr;
    }

    return (CPointCloudFrame::C3DRGBPixel*)CGeneralFrame::GetData();
}

inline const CPointCloudFrame::C3DRGBConfPixel*
CPointCloudFrame::Get3DRGBConfData() const {
    if (mFormat != CPointCloudFrame::e3DPointsRGBConf) {
        return nullptr;
    }

    return (CPointCloudFrame::C3DRGBConfPixel*)CGeneralFrame::GetData();
}

////////////////////////////////////////////////////////////////////////
///
///                                    DEPRICATED INTERFACES
///
////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
/// \brief    Represents  an CPointCloudFrame3D Frame that is  provided  by
/// InuDev PointCloud stream
///
////////////////////////////////////////////////////////////////////////
class CPointCloudFrame3D : public CPointCloudFrame {
   public:
    struct CPointCloudPixel : public CPoint3D {};

    /// \brief    Constructs empty (zero size) frame
    INUSTREAMS_API CPointCloudFrame3D() {}

    /// \brief    Constructor.
    /// \param[in] iNumOfPoints   Number Of Points
    /// \param[in] iFormat        Image format (which defines  the number of
    /// bytes  per pixel).
    INUSTREAMS_API CPointCloudFrame3D(unsigned int iNumOfPoints) {}

    /// \brief    Destructor.
    INUSTREAMS_API virtual ~CPointCloudFrame3D() {}

    /// \brief    Non mutable pointer to XYZPoint
    INUSTREAMS_API const CPointCloudPixel* GetData() const { return nullptr; }

    ///// \brief    Number of bytes that are used to represent each point.
    INUSTREAMS_API unsigned int BytesPerPoint() const { return 0; }

   protected:
    using CPointCloudFrame::mExposureParams;
};

////////////////////////////////////////////////////////////////////////
/// \brief    Represents  an CPointCloudFrame3D Frame that is  provided  by
/// InuDev PointCloud stream
///
////////////////////////////////////////////////////////////////////////
class CPointCloudFrame3DConf : public CPointCloudFrame {
   public:
    struct CPointCloudPixel : public CPoint4D {};

    /// \brief    Constructs empty (zero size) frame
    INUSTREAMS_API CPointCloudFrame3DConf() {}

    /// \brief    Constructor.
    /// \param[in] iNumOfPoints   Number Of Points
    /// \param[in] iFormat        Image format (which defines  the number of
    /// bytes  per pixel).
    INUSTREAMS_API CPointCloudFrame3DConf(unsigned int iNumOfPoints) {}

    /// \brief    Destructor.
    INUSTREAMS_API virtual ~CPointCloudFrame3DConf() {}

    /// \brief    Non mutable pointer to XYZPoint
    INUSTREAMS_API const CPointCloudPixel* GetData() const { return nullptr; }

    ///// \brief    Number of bytes that are used to represent each point.
    INUSTREAMS_API unsigned int BytesPerPoint() const { return 0; }

   protected:
    using CPointCloudFrame::mExposureParams;
};

////////////////////////////////////////////////////////////////////////
/// \brief    Represents  an CPointCloudFrame3DRGB Frame that is
///             provided  by InuDev PointCloud stream
///
////////////////////////////////////////////////////////////////////////
class CPointCloudFrame3DRGB : public CPointCloudFrame {
   public:
    struct CPointCloudPixel : public CPoint3D {
        byte R;
        byte G;
        byte B;
    };

    /// \brief    Constructs empty (zero size) frame
    INUSTREAMS_API CPointCloudFrame3DRGB() {}

    /// \brief    Constructor.
    /// \param[in] iNumOfPoints   Number Of Points
    /// \param[in] iFormat        Image format (which defines  the number of
    /// bytes  per pixel).
    INUSTREAMS_API CPointCloudFrame3DRGB(unsigned int iNumOfPoints) {}

    /// \brief    Destructor.
    INUSTREAMS_API virtual ~CPointCloudFrame3DRGB() {}

    /// \brief    Non mutable pointer to XYZPoint
    INUSTREAMS_API const CPointCloudPixel* GetData() const { return nullptr; }

    ///// \brief    Number of bytes that are used to represent each point.
    INUSTREAMS_API unsigned int BytesPerPoint() const { return 0; }

   protected:
    using CPointCloudFrame::mExposureParams;
};

////////////////////////////////////////////////////////////////////////
/// \brief    Represents  an CPointCloudFrame3DRGB Frame that is
///             provided  by InuDev PointCloud stream
///
////////////////////////////////////////////////////////////////////////
class CPointCloudFrame3DRGBConf : public CPointCloudFrame {
   public:
    struct CPointCloudPixel : public CPoint3D {
        byte R;
        byte G;
        byte B;
        byte Confidence;
    };

    /// \brief    Constructs empty (zero size) frame
    INUSTREAMS_API CPointCloudFrame3DRGBConf() {}

    /// \brief    Constructor.
    /// \param[in] iNumOfPoints   Number Of Points
    /// \param[in] iFormat        Image format (which defines  the number of
    /// bytes  per pixel).
    INUSTREAMS_API CPointCloudFrame3DRGBConf(unsigned int iNumOfPoints) {}

    /// \brief    Destructor.
    INUSTREAMS_API virtual ~CPointCloudFrame3DRGBConf() {}

    /// \brief    Non mutable pointer to XYZPoint
    INUSTREAMS_API const CPointCloudFrame3DRGBConf::CPointCloudPixel* GetData()
        const {
        return nullptr;
    }

    ///// \brief    Number of bytes that are used to represent each point.
    INUSTREAMS_API unsigned int BytesPerPoint() const { return 0; }
};
}  // namespace InuDev

#endif  // __POINT_CLOUD_FRAME_H__
