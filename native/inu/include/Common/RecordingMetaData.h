//
// File : RecordingMetaData.h
//

#ifndef __RECORDINGMEATDATA_H__
#define __RECORDINGMEATDATA_H__

#include <PersistentData.h>

#include <string>

#include "CalibrationData.h"
#include "CommonUtilitiesDefs.h"
#include "TiffFile.h"

#ifdef _MSC_VER
#pragma warning(disable : 4251)
#endif

namespace InuCommon {

////////////////////////////////////////////////////////////////////////
/// \brief    Interface for Tiff files
///
/// Role: Utility class which provides interface for reading and writing
///       Inuitive's images in Tiff files format.
///
/// Responsibilities:
///      1. Pack and save Inuitive's image as Tiff file
///      2. Read header, data or both
////////////////////////////////////////////////////////////////////////
struct COMMONUTILITIES_API CRecordingMetaData : public IPersistentData {
    std::string mSensorID;
    std::string mSensorName;
    std::string mDate;
    std::string mTime;
    std::string mSWVersion;
    unsigned int mFPS;

    unsigned int mCalibrationRev;
    CTiffFile::ESensorResolution mResoltion;
    InuDev::CCalibrationData mOpticalData;
    std::string mOpticalDataAsString;

    std::string mRegisters;

    virtual bool Load(
        const std::shared_ptr<InuCommon::IStorage>& storage) override;

    virtual bool Save(
        const std::shared_ptr<InuCommon::IStorage>& storage) override;
};

}  // namespace InuCommon

#ifdef _MSC_VER
#pragma warning(default : 4251)
#endif

#endif
