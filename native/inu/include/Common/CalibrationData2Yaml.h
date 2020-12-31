#ifndef __CALIBRATIONDATA2YAML_H__
#define __CALIBRATIONDATA2YAML_H__

#include "CommonUtilitiesDefs.h"
#include "CalibrationData.h"

namespace YAML { class Node;  }

namespace InuCommon
{
    ////////////////////////////////////////////////////////////////////////
    /// \brief    Persistency of CCalibrationData (Utilities class)  
    ///
    /// Role: Implementation of persistent behavior of Optical Data. Inherits InuDev::COpticalData
    ///
    /// Responsibilities: 
    ///      1. Knows how to load/save from files
    ////////////////////////////////////////////////////////////////////////
    class COMMONUTILITIES_API CCalibrationData2Yaml
    {
    public:


        /// \brief  Load optical data from stream
        /// \brief  Param[In]: iStream - input stream
        static bool Load(InuDev::CCalibrationData& oCalibration, std::istream& iStream);

        /// \brief  Save optical data to output stream system
        static bool Save(const InuDev::CCalibrationData& iCalibration, std::ostream& oStream);

    private:
       
        static bool LoadYaml(InuDev::CCalibrationData& oCalibration, YAML::Node &ymlFile);

        // load camera data from storage
        static bool ReadCameraFromYaml(const YAML::Node& iCameraNode, InuDev::CCalibrationData::COneSensorData::CCameraData &oCameraData);

    };
}

#endif
