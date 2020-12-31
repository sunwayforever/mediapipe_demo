
#ifndef __INUSENSORSDBDEFS_H__
#define __INUSENSORSDBDEFS_H__

#ifdef INUSENSORSDB_EXPORTS
    #ifdef __GNUC__
        #ifndef __linux__
            #define INUSENSORSDB_API __attribute__ ((dllexport))
        #else
            #define INUSENSORSDB_API __attribute__ ((visibility ("default")))
        #endif
    #else
        #define INUSENSORSDB_API __declspec(dllexport)
    #endif
#else
    #ifdef __GNUC__
        #ifndef __linux__
            #define INUSENSORSDB_API __attribute__ ((dllimport))
        #else
            #define INUSENSORSDB_API __attribute__ ((visibility ("default")))
        #endif
    #else
        #define INUSENSORSDB_API __declspec(dllimport)
    #endif
#endif

namespace InuCommon
{
    enum EInuSensorsDBErrors
    {
        eOK = 0,

        eInternalError,
        eCantRetrieveCalibrationFromTiff,
        eEmptyCalibrationDir,

        // InuModels 
        eBadFileRevision = 100,    /// File revision is not updated
        eFileOpen,           /// Can't open input file 
        eModelNameNotFound,  /// ModelName not found in data base file	

        // CalibrationData
        eFileNotFound = 200,
        eMetaDataFileNotFound,
        eRegistersFileNotFound,
        eMissingKeyInRegistersFile,
        eMissingTemperatures,
        eNoRevisions,
        eOpticalDataFileNotFound,
        eOpticalDataFileMissingValue,
        eFailedToReadConfigIreg,
        eUnexpectedEmptyCalibrations,
        eNoValidResolution,
        eSaveOpticalData,

        // Interpolation errors
        eFailedToCreateFolder = 300,
        eFailedToRemoveFolder,
        eFailedToCreateMetaData,
        eNoRegisterDataForReSize,
        eInterpolationSize,
        eCantGetWarmOrCOld,
        eUpdateHotOrCold,
        eFolderAlreadyExists,

        // RunDSR errors
        eDSRFailed = 400,
        eYmlNotFound,
    };
}

#endif