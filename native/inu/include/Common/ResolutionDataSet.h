
#ifndef __RESOLUTIONDATASE_H__
#define __RESOLUTIONDATASE_H__

#include "InuSensorDBDefs.h"

#include "RegistersSet.h"
#include "LutsDataSet.h"
#include "CalibrationData.h"
#include "TiffFile.h"
#include "boost/filesystem/path.hpp"

#include <memory>

namespace boost { namespace filesystem { class path; } }

namespace InuCommon
{
    class CRegistersSet;

    ////////////////////////////////////////////////////////////////////////
    /// \brief    Calibration data of specific resolution
    ///
    /// Role: Represents calibration data for one resolution (Optical data, LUTs and Initials registers values)
    ///
    /// Responsibilities: 
    ///      1. Knows how to load/save from file system 
    ///      2. Access to calibration data members (Optical data, LUTs and Initials registers values)
    ////////////////////////////////////////////////////////////////////////
    class INUSENSORSDB_API CResolutionDataSet
    {

    public:
       
        CResolutionDataSet(int iActualTemperature, const std::string& iResolutionName);

        /// \brief  Copy constructor
        CResolutionDataSet(const CResolutionDataSet& iInput);

        virtual ~CResolutionDataSet() {}

        /// \brief  Load from file system
        EInuSensorsDBErrors Load(const boost::filesystem::path& iResolutionSetDir, InuCommon::CTiffFile::EHWType iHwType, bool iLoadOdpReg, const InuDev::CCalibrationData* originalOptical);

        /// \brief    brief Get calibration LUT, pair of binary buffer and its size.
        /// 
        /// If LUT has not been loaded yet from DB, then it loads it and returns the loaded data 
        /// \param[in] iType    LUT type.
        /// \param[in] iLeftEye    Which eye (left/right).
        /// \return const std::pair<unsigned char*, unsigned int>&    If it fails then LUT size is 0.
        const std::vector<uint8_t>& GetLUT(CLutsDataSet::ELutType iType, bool iLeftEye, uint32_t iLutIndex = 0) const {
            return mLutsDataSet.GetLUT(iType, iLeftEye, iLutIndex);
        }
        
        /// \brief  Get the optical data and update the sensors ID
        ///
        ///         Backward compatibility: NU4000B0 with old optical data file (ImageParameters.ini), should be removed someday
        /// \brief  Param[In]: iStereoChannels - IR right and left sensors ID (of connected model)
        /// \brief  Param[In]: iFeChannels - FE right and left sensors ID (of connected model)
        /// \brief  Param[In]: iRgbChannel - RGB sensors ID (of connected model)
        std::shared_ptr<InuDev::CCalibrationData> GetOpticalDataAndUpdateSensorsID(
            const std::pair<int, int>& iStereoChannels,
            const std::pair<int, int>& iFeChannels,
            const int iRgbChannel);

        std::shared_ptr<InuDev::CCalibrationData> GetCalibrationData() const { return mCalibrationData; }

        std::shared_ptr<CRegistersSet> GetRegistersSet() const { return mRegistersSet; }

        std::shared_ptr<CRegistersSet> GetRegistersSet(){ return mRegistersSet; }

        static std::string Resolution2String(CTiffFile::ESensorResolution iResolution);

        const boost::filesystem::path& GetCalibrationDirectory() const { return mCalibrationDirectory; }

        void SetCalibrationDirectory(boost::filesystem::path iCalibrationDir) { mCalibrationDirectory = iCalibrationDir; }

        int GetActualTemperature() const { return mActualTemperature; }

        EInuSensorsDBErrors Save(const boost::filesystem::path& iCalibrationSetDir);
        
        EInuSensorsDBErrors SaveConfigIregAndOpticalData(const boost::filesystem::path& iCalibrationSetDir);

        const std::string& GetResolutionName() const { return mResolutionName; }

    private:


#ifdef _MSC_VER
#pragma warning(disable: 4251)
#endif
        std::shared_ptr<InuDev::CCalibrationData> mCalibrationData;

        std::shared_ptr<CRegistersSet> mRegistersSet;

        boost::filesystem::path mCalibrationDirectory;

        std::string mResolutionName;

#ifdef _MSC_VER
#pragma warning(default : 4251)
#endif

        CLutsDataSet mLutsDataSet;

        int mActualTemperature;

    };

    inline std::string CResolutionDataSet::Resolution2String(CTiffFile::ESensorResolution iResolution)
    {
        std::string result;
        switch (iResolution)
        {
            case CTiffFile::eVerticalBinning:
                result = "VerticalBinning";
                break;
            case CTiffFile::eBinning:
                result = "Binning";
                break;
            case CTiffFile::eFull:
                result = "Full";
                break;
            default:
                break;
        }

        return result;
    }

}
#endif
