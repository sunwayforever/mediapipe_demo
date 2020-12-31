#include <math.h>
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>

#include <vector>

#include "OSUtilities.h"
#include "SensorsControl.h"

enum eCVE_SensorID { eSensor_NAN, eSensorCVL, eSensorCVR, eSensorWebcam };

enum eCVE_M3SensorRegisters {
    eSReg_EmptyReg = 0,
    eSReg_Exposure,
    eSReg_GlobalGain,
    eSReg_RedGain,
    eSReg_GreenGain,
    eSReg_BlueGain
};

enum eCVE_SensorControlOperationOn {
    RIGHT_SENSOR = 0,
    LEFT_SENSOR,
    BOTH_SENSORS
};

struct eCVE_SensorPresetingColorationDefinition {
    int exposure;
    int globalGain;
    int blueGain;
    int redGain;
    int greenGain;
    bool preseting_set;
};

// Maximum number of registers requests is (2 x number of registers):
const int MAX_M3_REG_REQUESTS = 10;

struct sCVE_SensorRegisterRequest {
    // sCVE_SensorRegisterRequest() {readReg = eSReg_EmptyReg;}
    eCVE_M3SensorRegisters RegId;
    eCVE_SensorID sensorId;
    // SENSORS_CTRLG_sensorRegFile regFile[2];
    unsigned int regData;
};

struct sCVE_AEGC_Config {
    int Exposure_GL_maxValHigh;
    int Exposure_GL_minValHigh;
    int Exposure_GL_minValLow;
    int AnalogGain_GL_maxValHigh;
    int AnalogGain_GL_maxValLow;
    int AnalogGain_GL_minValLow;
    double AnalogGain_GL_minEfficienyVal;
    int DigitalGain_GL_maxValHigh;
    int DigitalGain_GL_maxValLow;
    int DigitalGain_GL_minValLow;
    double DigitalGain_GL_maxEfficienyVal;
    double DigitalGain_GL_minEfficienyVal;
};

struct sCVE_SensorControlManager {
    sCVE_SensorControlManager() { set_default(); }

    eCVE_SensorControlOperationOn sensorControlOperationOn;
    bool autoExposure_Set;
    bool analogAGC_Set;
    bool digitalAGC_Set;
    bool sensorControlInit_Finished;
    bool sensorControlROI_OfGaze_set;
    bool sensorControlROI_OfFingers_set;
    bool sensorControlROI_OfHead_set;
    int num_read_requests;
    int num_write_requests;

    sCVE_SensorRegisterRequest readRegs[MAX_M3_REG_REQUESTS];
    sCVE_SensorRegisterRequest writeRegs[MAX_M3_REG_REQUESTS];
    sCVE_AEGC_Config gazeAEGC_Config;
    sCVE_AEGC_Config fingersAEGC_Config;
    sCVE_AEGC_Config headAEGC_Config;
    eCVE_SensorPresetingColorationDefinition presetingColorationDefinition;

    void set_default(void) {
        autoExposure_Set = false;
        analogAGC_Set = false;
        digitalAGC_Set = false;
        sensorControlInit_Finished = false;
        sensorControlROI_OfGaze_set = false;
        sensorControlROI_OfFingers_set = false;
        sensorControlROI_OfHead_set = false;
        num_read_requests = 0;
        num_write_requests = 0;
        presetingColorationDefinition.preseting_set = false;
    }
};

//////////////////////////////////////
// CVESC_ResetSensorControlManager
COMMONALGO_API void CVESC_ResetSensorControlManager(
    sCVE_SensorControlManager& scmgr);
//////////////////////////////////////
// CVE_GetRegisterReadRequest
COMMONALGO_API sCVE_SensorRegisterRequest* CVESC_GetRegisterReadRequest(
    sCVE_SensorControlManager& SCM, eCVE_M3SensorRegisters RegID,
    eCVE_SensorID SensorID);
//////////////////////////////////////
// CVE_GetRegisterWriteRequest
COMMONALGO_API sCVE_SensorRegisterRequest* CVESC_GetRegisterWriteRequest(
    sCVE_SensorControlManager& SCM, eCVE_M3SensorRegisters RegID,
    eCVE_SensorID SensorID);
//////////////////////////////////////////////////////////////
// Add Register Read request in 'from' to  'to'
// if the same request is already there, do not add anything.
COMMONALGO_API bool CVESC_AddRegReadRequests(sCVE_SensorControlManager& from,
                                             sCVE_SensorControlManager& to);
//////////////////////////////////////////////////////////////
// Add Register Write request in 'from' to  'to'
// if the same request is already there, do not add anything.
// in case of contradicting requests, return false (failure)
COMMONALGO_API bool CVESC_AddRegWriteRequests(sCVE_SensorControlManager& from,
                                              sCVE_SensorControlManager& to);
//////////////////////////////////////////////////////////////////////
// CopyRegReadRequests
COMMONALGO_API void CVESC_CopyRegReadRequests(
    sCVE_SensorControlManager& src, sCVE_SensorControlManager& target);
//////////////////////////////////////////////////////////////////////
// CopyRegWriteRequests
COMMONALGO_API void CVESC_CopyRegWriteRequests(
    sCVE_SensorControlManager& src, sCVE_SensorControlManager& target);
///////////////////////////////////////////
// GetRegValue
COMMONALGO_API int CVESC_GetRegValue(sCVE_SensorControlManager& RegResults,
                                     sCVE_SensorRegisterRequest& targetReg);
///////////////////////////////////////////
// SetRegisterWriteRequest
COMMONALGO_API bool CVESC_SetRegisterWriteRequest(
    sCVE_SensorControlManager& commonsc, eCVE_M3SensorRegisters reg,
    eCVE_SensorID sens, int regData);
