/****************************************************************************
 *
 *   FileName: sensors_control.h
 *
 *   Author: Oren K.
 *
 *   Date:
 *
 *   Description:  Sensors Control Module Header
 *
 *
 ****************************************************************************/
#ifndef SENSORS_CONTROL
#define SENSORS_CONTROL

#include <stdint.h>

#include "CommonAlgo.h"

#ifdef _MSC_VER
#include <windows.h>
#endif

/****************************************************************************
 ***************      G L O B A L   T Y P E D E F S           ***************
 ****************************************************************************/
#ifdef __GNUC__
typedef uint32_t UINT32;
typedef uint16_t UINT16;
typedef uint8_t UINT8;
typedef int8_t INT8;
#endif

typedef enum {
    MASTER_E = 0,
    SLAVE_E,
    NUM_OF_SENSORS,
} SENSORS_CTRLG_sensorsE;

typedef struct sensor_reg_file {
    /* sensor's registers */
    UINT16 exposureTimeREG;
    UINT16 gainREG;
} SENSORS_CTRLG_sensorRegFile;

typedef enum sensors_ctrl_status {
    SENSORS_CTRL_FIND_RESOLUTION_E = 0,
    SENSORS_CTRL_PROCESS_E,
} SENSORS_CTRLG_sensorsCtrlStateE;

/****************************************************************************
 ***************     G L O B A L         F U N C T I O N S    ***************
 ****************************************************************************/
COMMONALGO_API int SENSORS_CTRLG_initFrm(UINT16 imgWidth, UINT16 imgHeight,
                                         UINT8 *imgP);
COMMONALGO_API void SENSORS_CTRLG_initROI(UINT16 x, UINT16 y, UINT16 width,
                                          UINT16 height);
COMMONALGO_API void SENSORS_CTRLG_configParams(
    UINT8 absDiffMeanLR, UINT8 absDiffMedianLR, UINT8 absDiffMedAvgLL,
    UINT8 absDiffMedAvgRR, UINT8 minGlobalAvg, UINT8 maxGlobalAvg);
COMMONALGO_API void SENSORS_CTRLG_configDefaultParams();
COMMONALGO_API void SENSORS_CTRLG_fflushDebugFile();
COMMONALGO_API void SENSORS_CTRLG_loadConfig();
COMMONALGO_API SENSORS_CTRLG_sensorsCtrlStateE SENSORS_CTRLG_sensorsControl(
    SENSORS_CTRLG_sensorRegFile oResult[2], UINT32 *frmActivationP);
COMMONALGO_API void SENSORS_CTRLG_depthMode(
    SENSORS_CTRLG_sensorRegFile regFile[2]);

COMMONALGO_API UINT32 SENSORS_CTRLG_setFrmRoi(SENSORS_CTRLG_sensorsE side,
                                              UINT16 x, UINT16 y, UINT16 width,
                                              UINT16 height);
COMMONALGO_API UINT32 SENSORS_CTRLG_setUserDefMode(
    SENSORS_CTRLG_sensorRegFile regFile[2], UINT16 gainMaster,
    UINT16 exposureTimeMaster, UINT16 gainSlave, UINT16 exposureTimeSlave);

#endif  //#define SENSORS_CONTROL
