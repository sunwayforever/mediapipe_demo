
#ifndef __INUSENSOR__
#define __INUSENSOR__

#include <stdint.h>

#include <climits>
#include <string>
#include <vector>

#include "CalibrationDataAll.h"
#include "InuModel.h"
#include "InuSensorDBDefs.h"
#include "PersistentData.h"
#include "boost/filesystem/path.hpp"

#ifdef _MSC_VER
#pragma warning(disable : 4251)
#endif

namespace InuCommon {
////////////////////////////////////////////////////////////////////////
/// \brief    CInuDevice data
///
/// Role: Represents one InuSensor
/// InuSensors is module/device produced by Inuitive and should be access by any
/// Host SW component
///
/// Responsibilities:
///      1. SensorID (unique)
///      2. Different calibrations data
////////////////////////////////////////////////////////////////////////
class INUSENSORSDB_API CInuDevice {
    friend class CInuDeviceDB;

   public:
    static const int UNDEFIEND_CALIB_TEMPERATURE;
    static const unsigned int UNDEFIEND_REVISION;

    // Constructor and destructor
    CInuDevice(const std::string& iSensorID, const std::string& iSensorModel,
               InuCommon::CTiffFile::EHWType iHwType);

    virtual ~CInuDevice() {}

    /// \brief    Initialize the object and prepare it for working, must be call
    /// after construction
    EInuSensorsDBErrors Init();

    /// \brief    Load calibration data
    /// \param[in] iRequiredTemperatures    List of all temperatures that should
    /// have calibration data.
    ///                                     All missing calibration should be
    ///                                     calculated by the load function
    /// \param[in] iDirectoryName    Enable to load calibration files from
    /// different directory. \param[in] iSaturationMin   Minimum Saturation
    /// value for interpolation/extrapolation \param[in] iSaturationMax Maximum
    /// Saturation value for interpolation/extrapolation \param[in]
    /// iDirectoryName   Calibration directory name \param[in]
    /// iTemperatureCompenstation   Whether to invoke temperature compensation
    /// or not
    EInuSensorsDBErrors LoadCalibration(
        const std::vector<int>& iRequiredTemperatures, int iSaturationMin,
        int iSaturationMax, const std::string& iDirectoryName,
        bool iTemperatureCompenstation);

    /// \brief    Retrieve specific calibration data
    /// \param[out] oResult    Retrieved calibration data.
    /// \param[in] iResolution    Resolution must be provided.
    /// \return bool    nullptr if no calibration was found.
    std::shared_ptr<const CTemperatureDataSet> GetTemperatureDataSet(
        int iTemprature = UNDEFIEND_CALIB_TEMPERATURE,
        unsigned int iRevision = UNDEFIEND_REVISION);

    /// \brief   is this calibration was done for different temperatures
    bool TemperatureCalibration() const;

    //// Getters and Setters
    ///
    const std::string& SensorID() const { return mSensorID; }

    const CInuModel& SensorModel() const { return mSensorModel; }

    const std::string& GetSensorModelName() const { return mSensorModelName; }

    InuCommon::CTiffFile::EHWType HwType() const { return mHwType; }

    const std::string& PartNumber() const { return mPartNumber; }
    void SetPartNumber(const std::string& iPartNumber) {
        mPartNumber = iPartNumber;
    }

    const std::string& SensorRevision() const { return mSensorRevision; }
    void SetSensorRevision(const std::string& iSensorRevision) {
        mSensorRevision = iSensorRevision;
    }

    unsigned int GetBootID() const { return mBootId; }
    void SetBootID(unsigned int iInput) { mBootId = iInput; }

    uint32_t GetFwVerId() const { return mFwVerId; }
    void SetFwVerId(uint32_t iInput) { mFwVerId = iInput; }

    unsigned int GetRevisionID() const { return mRevisionID; }
    void SetRevisionID(unsigned int iInput) { mRevisionID = iInput; }

    const std::vector<int>& GetMasterSensorId() const {
        return mMasterSensorId;
    }
    void SetMasterSensorId(const std::vector<int>& iID) {
        mMasterSensorId = iID;
    }

    const std::vector<int>& GetSlaveSensorId() const { return mSlaveSensorId; }
    void SetSlaveSensorId(const std::vector<int>& iID) { mSlaveSensorId = iID; }

    const std::vector<int>& GetISPVersion() const { return mISPVersion; }
    void SetISPVersion(const std::vector<int>& iID) { mISPVersion = iID; }

    unsigned int GetVersionID() const { return mVersionID; }
    void SetVersionID(unsigned int iInput) { mVersionID = iInput; }

    unsigned int GetUSBSpeed() const { return mUSBSpeed; }
    void SetUSBSpeed(unsigned int iInput) { mUSBSpeed = iInput; }

    uint32_t GetCalibrationDataSize() const { return mCalibrationDataSize; }
    void SetCalibrationDataSize(uint32_t iInput) {
        mCalibrationDataSize = iInput;
    }

    uint32_t GetCalibrationDataFormat() const { return mCalibrationDataFormat; }
    void SetCalibrationDataFormat(uint32_t iInput) {
        mCalibrationDataFormat = iInput;
    }

    uint64_t GetLastBurnTime() const { return mLastBurnTime; }
    void SetLastBurnTime(uint64_t iInput) { mLastBurnTime = iInput; }

    const std::string& CalibrationVersion() const {
        return mCalibrationVersion;
    }

    uint64_t GetBootfixTimestamp() const { return mBootfixTimestamp; }
    void SetBootfixTimestamp(uint64_t iInput) { mBootfixTimestamp = iInput; }

    const std::string& GetBootfixVersion() const { return mBootfixVersion; }
    void SetBootfixVersion(const std::string& iInput) {
        mBootfixVersion = iInput;
    }

    bool GetDualSensorsMaster() const { return mDualSensorsMaster; }
    void SetDualSensorsMaster(bool iIsMaster) {
        mDualSensorsMaster = iIsMaster;
    }

    /// \return bool    true if there is a match between SWRevision and local
    /// calibration folder
    bool LocalPath() const;

   private:
    /// \brief  Sensor general information

    int mVersionID;
    int mRevisionID;
    uint32_t mFwVerId;
    int mUSBSpeed;
    std::vector<int> mMasterSensorId;
    std::vector<int> mSlaveSensorId;
    std::vector<int> mISPVersion;

    std::string mSensorID;
    std::string mSensorModelName;
    CInuModel mSensorModel;
    std::string mPartNumber;
    std::string mSensorRevision;

    uint32_t mCalibrationDataSize;
    uint32_t mCalibrationDataFormat;

    unsigned int mBootId;
    uint64_t mBootfixTimestamp;
    std::string mBootfixVersion;

    InuCommon::CTiffFile::EHWType mHwType;

    /// \brief  If true then either it's not a Dual Sensors configuration or
    /// this is the Master InuSesnor of Dual Sensors configuration
    bool mDualSensorsMaster;

    /// \brief  A counter that is increased when data is burned on flash
    uint64_t mLastBurnTime;

    /// \brief  A counter that is increased when data is burned on flash
    std::string mCalibrationVersion;

    /// \brief  Collection of all calibrations.
    std::shared_ptr<CCalibrationDataAll> mCalibrationDataAll;

    /// \brief    Returns local path of current sensor (if exists).
    /// \param[in] iSensorID    Sensor ID which is used to build directory name.
    /// \return std::string    Local path of current sensor's data or empty
    /// string if it doesn't exist
    static std::string LocalPath(const std::string& iSensorID);
};

inline bool CInuDevice::TemperatureCalibration() const {
    return (mCalibrationDataAll != nullptr) &&
           mCalibrationDataAll->TemperatureCalibration();
}

}  // namespace InuCommon

#endif
