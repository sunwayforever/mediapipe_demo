
#ifndef __LUTSDATASET_H__
#define __LUTSDATASET_H__

#include "InuSensorDBDefs.h"
#include "TiffFile.h"


#include <string>
#include <utility>
#include <vector>

namespace boost { namespace filesystem { class path; } }

namespace InuCommon
{
    ////////////////////////////////////////////////////////////////////////
    /// \brief    All LUTs of one calibration set  
    ///
    /// Role: Represents all LUTs that should be loaded to chip
    ///
    /// Responsibilities: 
    ///      1. Knows how to load/save from LUT files
    ///      2. Access to all LUTs
    ////////////////////////////////////////////////////////////////////////
    class INUSENSORSDB_API CLutsDataSet 
    {
    public:

        /// \brief  Different types of LUT
        enum ELutType
        {
            eIntensityBalance = 0,
            eDistortionAndRectification,
            eBPC,
            eFIR,
            eWebcamCeva,
            eNumberOfTypes
        };
        
        CLutsDataSet() {}
        virtual ~CLutsDataSet() {}

         /// \brief    brief Get calibration LUT, pair of binary buffer and its size.
        /// 
        /// If LUT has not been loaded yet from DB, then it loads it and returns the loaded data 
        /// \param[in] iType    LUT type.
        /// \param[in] iLeftEye    Which eye (left/right).
        /// \param[in] iLutIndex   The Lut index.
        /// \return const std::pair<unsigned char*, unsigned int>&    If it fails then LUT size is 0.
        const std::vector<uint8_t>& GetLUT(ELutType iType, bool iLeftEye, uint32_t iLutIndex = 0) const { return mLuts[iType][(iLeftEye ? 0 : 1) + iLutIndex * 2];
        }
       
        EInuSensorsDBErrors Load(const boost::filesystem::path& iCalibrationSetDir, InuCommon::CTiffFile::EHWType iHwType);

        EInuSensorsDBErrors Save(const boost::filesystem::path& iCalibrationSetDir);

    private:


        /// \brief  All different types of LUTS. Value is null and size is 0, if LUT has not been loaded yet.
#ifdef _MSC_VER
#pragma warning(disable : 4251)
#endif
        std::vector<uint8_t>  mLuts[eNumberOfTypes][8];

        static std::string sLutsNames[eNumberOfTypes][8];

#ifdef _MSC_VER
#pragma warning(default : 4251)
#endif

        EInuSensorsDBErrors LoadLUT(const boost::filesystem::path& iCalibrationSetDir, ELutType iType, bool iLeftEye, uint32_t iFileIndex = 0);
    };

}
#endif
