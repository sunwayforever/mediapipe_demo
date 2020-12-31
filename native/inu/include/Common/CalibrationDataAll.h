
#ifndef __CALIBRATIONDATAALL__
#define __CALIBRATIONDATAALL__

#include "InuSensorDBDefs.h"
#include "ResolutionDataSet.h"

#include "TiffFile.h"

#include <memory>
#include <map>

namespace boost { namespace filesystem { class path; } }

#ifdef _MSC_VER
#pragma warning(disable : 4251)
#endif

namespace InuCommon
{
    class CInuDevice;
    class CTemperatureDataSet;
    class CMissingCalibrationsAlg;

    ////////////////////////////////////////////////////////////////////////
    /// \brief    One calibration data of InuSensor
    ///
    /// Role: Represents All calibration data (there might be different calibrations per InuSensor - per temperature)
    ///
    /// Responsibilities: 
    ///      1. Knows how to load/save from calibration folder 
    ///      2. Access to calibration data members (Optical data, LUTs and Initials registers values)
    ////////////////////////////////////////////////////////////////////////
    class INUSENSORSDB_API CCalibrationDataAll : public std::enable_shared_from_this<CCalibrationDataAll>
    {

    public:

        CCalibrationDataAll();

        virtual ~CCalibrationDataAll();

        /// \brief  Load calibration data from file system
        EInuSensorsDBErrors Load(const boost::filesystem::path& iCalibrationDir, const CInuDevice& iParentSensorconst, const std::vector<int>& iRequiredTemperatures, int iSaturationMin, int iSaturationMax, bool iTemperatureCompenstation);

        /// \brief    Retrieve specific calibration data
        /// \param[in] iResolution    Resolution must be provided.
        /// \param[in] iTemprature    Calibration revision that should be retrieved.
		/// \param[in] iPreciseTemp   If true, returns the exact temp calibration found. Otherwise - closest.
        /// \return bool    nullptr if no calibration was found.
        std::shared_ptr<const CTemperatureDataSet>  GetCalibrationData(int iTemprature, bool iPreciseTemp) const;
        
        const std::map<int, std::shared_ptr<CTemperatureDataSet>>& GetTemperatureDataSets() const { return mTemperatureDataSets; }

        std::map<CTiffFile::ESensorResolution, std::pair<unsigned int, unsigned int>> GetMinSizeForAllResolutions() const { return mMinSizePerResolution; }

        bool  CalibrationDataExist(int iTemprature) const;

        void AddCalibrationData(std::shared_ptr<CTemperatureDataSet> iCalibrationDataset, bool iOverWrite = false);

        const std::string& GetSensorName() const { return mSensorName; }

        /// \brief   if this calibration was done for different temperatures
        bool TemperatureCalibration() const;

        void UpdateMinSizes(std::shared_ptr<CTemperatureDataSet> iCalibrationToUpdateWith);

    private:

        static const std::string REVISION_FOLDER_PREFIX;

        /// \brief  Map of all available calibrations for different temperatures
        std::map<int, std::shared_ptr<CTemperatureDataSet>> mTemperatureDataSets;

        /// \brief  Algorithm that calculates calibration data on the fly
        CMissingCalibrationsAlg*  mMissingCalibrationsAlg;

        /// \brief  Minimum size of each Resolutions
        std::map<CTiffFile::ESensorResolution, std::pair<unsigned int, unsigned int>> mMinSizePerResolution;
		
        std::string mSensorName;

        /// \brief    Retrieve meta data from calibration file.
        /// \param[in] iCalibrationDir    Calibration directory
        /// \return true    if additional data was found
        bool LoadFromFile(const boost::filesystem::path& iCalibrationDir, const std::string& iSensorName);

        /// \brief    Load from revision directory (only for FileSystem DB)
        /// 
        /// This function scan all temperatures directories and load from the match directory 
        /// \param[in] iCalibrationDir    Calibration folder name.
        /// \param[in] iTemprature    Required temperature.
        /// \return EInuSensorsDBErrors    
        EInuSensorsDBErrors ReadTemperaturesCalibrations(const boost::filesystem::path& iCalibrationDir, InuCommon::CTiffFile::EHWType iHwType);

        /// \brief    Load from revision directory (only for FileSystem DB)
        /// 
        /// This function scan all revision directories and load from the first directory 
        /// \param[in] iCalibrationDir    Calibration folder name.
        /// \param[in] iRevision    Required revision.
        /// \return EInuSensorsDBErrors   
        EInuSensorsDBErrors ReadRevisionCalibration(const boost::filesystem::path& iCalibrationDir, InuCommon::CTiffFile::EHWType iHwType);

    };
}

#ifdef _MSC_VER
#pragma warning(default : 4251)
#endif

#endif
