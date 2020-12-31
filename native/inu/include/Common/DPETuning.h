/* DPETuning.h */

#ifndef DPE_TUNING_H
#define DPE_TUNING_H

#include "CommonAlgo.h"

typedef unsigned char  UINT8;
typedef unsigned short UINT16;
typedef unsigned int   UINT32;

#ifdef __cplusplus
extern "C" {
#endif

#define DPETUNEG_MAX_REGISTER_LIST 20

typedef enum 
{
   DPETUNING_OK_E = 0,
   DPETUNING_FAIL_E,
} DPETUNEG_statusE;

typedef enum
{
   DPETUNING_RIGHT_SENSOR_E = 0,
   DPETUNING_LEFT_SENSOR_E,
   DPETUNING_BOTH_SENSORS_E,
} DPETUNEG_sensorNumberE;

typedef struct dpe_tuning_register
{
   UINT32 regAddr;
   UINT32 regData;
   UINT32 regSize;
   DPETUNEG_sensorNumberE sensorNum;
} DPETUNEG_registerT;

typedef struct dpe_tuning_registers_list
{
   DPETUNEG_registerT RegisterGet[DPETUNEG_MAX_REGISTER_LIST];
	DPETUNEG_registerT RegisterSet[DPETUNEG_MAX_REGISTER_LIST];
} DPETUNEG_regListT;

typedef struct dpe_tuning_output_params
{
	DPETUNEG_regListT outRegisters;
   DPETUNEG_statusE  status;
} DPETUNEG_outParamsT;

typedef struct dpe_tuning_input_params
{
   DPETUNEG_regListT inRegisters;
   UINT16            *pYUV;
   UINT16            *pDisparity;
   UINT16            width;
   UINT16            height;
   UINT32            timeStamp;   
} DPETUNEG_inParamsT;

COMMONALGO_API void DPETUNEG_process( DPETUNEG_inParamsT *inParams, DPETUNEG_outParamsT *outParams );

#ifdef __cplusplus
}
#endif

#endif
