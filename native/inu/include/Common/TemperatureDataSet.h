
#ifndef __CALIBRATIONDATASET_H__
#define __CALIBRATIONDATASET_H__

#include "InuSensorDBDefs.h"

#include "TiffFile.h"
#include "ResolutionDataSet.h"
#include "RegistersSet.h"

#include "boost/filesystem/path.hpp"

#include <memory>
#include <string>
#include <map>

namespace InuCommon
{
    ////////////////////////////////////////////////////////////////////////
    /// \brief    Calibration data for one temperature
    ///
    /// Role: Represents one calibration data set for one temperature 
    ///
    /// Responsibilities: 
    ///      1. Knows how to load/save from file 
    ///      2. Access to calibration data members (Optical data, LUTs and Initials registers values)
    ////////////////////////////////////////////////////////////////////////
    class INUSENSORSDB_API CTemperatureDataSet 
    {
    public:

        CTemperatureDataSet(int iActualTemperature) :
            mResolutionsDataSets(),
            mActualTemperature(iActualTemperature), 
            mCalculated(true){}

        /// \brief  Copy constructor
        CTemperatureDataSet(const CTemperatureDataSet& iInput);

        virtual ~CTemperatureDataSet() {}

        /// \brief  Load from file system
        EInuSensorsDBErrors Load(const boost::filesystem::path& iCalibrationSetDir, InuCommon::CTiffFile::EHWType iHwType, bool iLoadOdpReg);

        /// \brief  Save calibration data for RunDSR to file system
        EInuSensorsDBErrors SavePreliminaryData();

        /// \brief  remove calibration data in case DSR failed
        EInuSensorsDBErrors RemovePreliminaryData();

        /// \brief  Save calibration data after RunDSR to file system
        EInuSensorsDBErrors SaveDSROutput() const;

        /// \brief  Update calibration data accroding to minimum sizes and given resolution and save it to disk. iIsOriginalData is Hot or Cold calibrations.
        EInuSensorsDBErrors UpdateSize(const std::string& iModelName, 
            const std::map<CTiffFile::ESensorResolution, std::pair<unsigned int, unsigned int>>& inewSizes, CTiffFile::ESensorResolution iResolution, bool iIsOriginalData);

        /// \brief  Update calibration data accroding to minimum sizes which iterates over all resolutions and save it to disk
        EInuSensorsDBErrors UpdateSize(const std::string& iModelName,
            const std::map<CTiffFile::ESensorResolution, std::pair<unsigned int, unsigned int>>& inewSizes, bool iIsOriginalData);


        // Getter and Setters
        // ------------------
        std::shared_ptr<const CResolutionDataSet> GetResolution(CTiffFile::ESensorResolution iResolution) const;
        const std::map<std::string, std::shared_ptr<CResolutionDataSet>>& GetResolutionDataSets() const { return mResolutionsDataSets; }

        int GetActualTemperature() const { return mActualTemperature; }
        void SetActualTemperature(int iTemperature)  { mActualTemperature = iTemperature; }

        bool IsCalculated() const { return mCalculated; }
        void SetIsCalculated(bool iIsCalculated) { mCalculated = iIsCalculated; }

        const boost::filesystem::path& GetCalibrationDir() const { return mCalibrationDir; }
        void SetCalibrationDir(const boost::filesystem::path& iCalibrationDir);

        /// \brief    Location of chip type related calibration files 
        static std::string  ChipTypeRelatedCalibrarionFolder(InuCommon::CTiffFile::EHWType iHwType);

    private:

#ifdef _MSC_VER
#pragma warning(disable: 4251)
#endif
        static const std::string CALCUATED_FOLDER;

        /// \brief  Map of all available calibrations for different temperatures
        std::map<std::string, std::shared_ptr<CResolutionDataSet>> mResolutionsDataSets;

        boost::filesystem::path mCalibrationDir;

#ifdef _MSC_VER
#pragma warning(default: 4251)
#endif

        /// \brief  Actual calibration temperature
        int mActualTemperature;

        /// \brief  True iff this isn't an original calibration but an estimated one
        bool mCalculated;

        /// \brief Update register data to new size: registers and metadata
        EInuSensorsDBErrors UpdateConfigIregRegisters(const std::string& iModelName, const std::pair<unsigned int, unsigned int> &inewSizes, CTiffFile::ESensorResolution iResolution);

    };

    inline std::string  CTemperatureDataSet::ChipTypeRelatedCalibrarionFolder(InuCommon::CTiffFile::EHWType iHwType)
    {
        return (iHwType == InuCommon::CTiffFile::EHWType::eNU4000B0 || iHwType == InuCommon::CTiffFile::EHWType::eNU4000C0)? "NU4K" : "HW";
    }

    inline std::shared_ptr<const CResolutionDataSet> CTemperatureDataSet::GetResolution(CTiffFile::ESensorResolution iResolution) const
    {
        std::string resStr = CResolutionDataSet::Resolution2String(iResolution);
        if (mResolutionsDataSets.find(resStr) != mResolutionsDataSets.end())
        {
            return mResolutionsDataSets.find(resStr)->second;
        }
        return nullptr;
    }

    inline void CTemperatureDataSet::SetCalibrationDir(const boost::filesystem::path& iCalibrationDir)
    {
        mCalibrationDir = iCalibrationDir;
        for (auto& elem : mResolutionsDataSets)
        {
            elem.second->SetCalibrationDirectory(iCalibrationDir);
        }
    }
}
#endif
