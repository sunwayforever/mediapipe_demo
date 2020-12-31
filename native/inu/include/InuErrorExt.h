/*
* File - InuErrorExt.h
*
* This file is part of the Inuitive SDK
*
* Copyright (C) 2014 All rights reserved to Inuitive
*
*/

#ifndef __INUERROREXT_H__
#define __INUERROREXT_H__

#include "StreamsExport.h"

#include "InuError.h"

namespace InuDev
{
    ///////////////////////////////////////////////////////////////////////
    // Enumerator: EErrorCode
    // Role: Error codes that may be returned by IAF
    ///////////////////////////////////////////////////////////////////////
    #define HOST_PID_MASK(pid)    (pid << 24)
    #define HOST_MID_MASK(mid)    (mid << 16)
    #define ERRG_HOST_MASK(error) error | HOST_PID_MASK(4)


    //Any error code added here must be added in the CreateErrorMap() method in the cpp file.
    enum EErrorCodeExt : int
    {
        eStatusOK =                     ERRG_HOST_MASK(0),
        eGeneralException =             ERRG_HOST_MASK(1),
        eSerializationException =       ERRG_HOST_MASK(2),
        eEventException =               ERRG_HOST_MASK(3),
        eSyncObjectException =          ERRG_HOST_MASK(4),
        eThreadException =              ERRG_HOST_MASK(5),
        eSharedMemoryException =        ERRG_HOST_MASK(6),
        eBufferAllocationException =    ERRG_HOST_MASK(7),
        eServiceNotFound =              ERRG_HOST_MASK(8),
        eIPCException =                 ERRG_HOST_MASK(9),
        eFileOpenError =                ERRG_HOST_MASK(10),
        eServiceFailure =               ERRG_HOST_MASK(11),
        eServiceNotInitaliazed =        ERRG_HOST_MASK(12),
        eUSBDisconnect =                ERRG_HOST_MASK(13),
        eUSBConnect =                   ERRG_HOST_MASK(14),
        eStreamDisconnected =           ERRG_HOST_MASK(15),
        eChannelIDInvalid =             ERRG_HOST_MASK(16),
        eNegativeDisparityOffset =      ERRG_HOST_MASK(17),
        eNoMatchingFormat =             ERRG_HOST_MASK(18),
        eUnexpectedInitParameter   =    ERRG_HOST_MASK(19),
        eWrongBootType             =    ERRG_HOST_MASK(20),
        eMessageUnhandled          =    ERRG_HOST_MASK(21),
        eModuleReady               =    ERRG_HOST_MASK(22),
        eModuleNotReady            =    ERRG_HOST_MASK(23),
        eMismatchBufferSize        =    ERRG_HOST_MASK(24),
        eFWGeneralError            =    ERRG_HOST_MASK(25),
        ePasswordError             =    ERRG_HOST_MASK(26),
        eMapNotEmptyError          =    ERRG_HOST_MASK(27),
        eModuleNotStarted          =    ERRG_HOST_MASK(28),
        eNotSupportedByChipRevision = ERRG_HOST_MASK(29),
        eFailedToUnregister = ERRG_HOST_MASK(30),
        eFailedToCloseStream = ERRG_HOST_MASK(31),
		eInvalidInterleavedType		= ERRG_HOST_MASK(32),
        eCropWidthShouldBe16multiplier = ERRG_HOST_MASK(33),
        eInvalidFrameFromService    = ERRG_HOST_MASK(34),

        // Storage errors
        eMisssingStorageException =     ERRG_HOST_MASK(200),
        eMisssingTableException=        ERRG_HOST_MASK(201),
        eMisssingKeyException=          ERRG_HOST_MASK(202),
        eSaveException =                ERRG_HOST_MASK(203),

        // NU3000 Driver Errors
        eInitException =                ERRG_HOST_MASK(400),
        eFinalizeException=             ERRG_HOST_MASK(401),
        eStartException =               ERRG_HOST_MASK(402),
        eStopException=                 ERRG_HOST_MASK(403),
        eAcquisitionException =         ERRG_HOST_MASK(404),
        eServiceStateException =        ERRG_HOST_MASK(405),
        eWrongWorkMessageType  =        ERRG_HOST_MASK(406),
        eFailedToLoadRegister =         ERRG_HOST_MASK(407),
        eCantGetInfoFromBothSensors =   ERRG_HOST_MASK(408),
        eFailedToWriteToSensorUVC   =   ERRG_HOST_MASK(409),
        eFailedToLoadLUT   =            ERRG_HOST_MASK(410),
        eFaileToRetrieveSensorID =      ERRG_HOST_MASK(411),
        eCevaReadError =                ERRG_HOST_MASK(412),
        eCevaLoadError =                ERRG_HOST_MASK(413),
        eSetFWParams =                  ERRG_HOST_MASK(414),
        eGetFWParams =                  ERRG_HOST_MASK(415),
        eStartFWAlgorithm =             ERRG_HOST_MASK(416),
        eReadRegister =                 ERRG_HOST_MASK(417),
        eFPSMissMatchError =            ERRG_HOST_MASK(418),
        eFailed2ConnectWithSensor =     ERRG_HOST_MASK(419),

        // Sensory
        eCreateServiceError =           ERRG_HOST_MASK(600),
        eCreateRegonizerError =         ERRG_HOST_MASK(601),
        eSettingRegonizerError =        ERRG_HOST_MASK(602),
        eInitRecognizerError =          ERRG_HOST_MASK(603),
        eCreateSearchError =            ERRG_HOST_MASK(604),
        eSettingSearchError =           ERRG_HOST_MASK(605),
        eRecognitionError =             ERRG_HOST_MASK(606),
        eRecognitionClippedError =      ERRG_HOST_MASK(607),
        eWaveFileError =                ERRG_HOST_MASK(608),
        eRecogSampleConvertError =      ERRG_HOST_MASK(609),
        eRecogPipeError =               ERRG_HOST_MASK(610),

        // injection
        eBadInjectionStream =           ERRG_HOST_MASK(700),
        eInjectionError =               ERRG_HOST_MASK(701),
        eFailedWriteInjectionError =    ERRG_HOST_MASK(702),

        //general stream errors
        eEventWaitTimeoutError =        ERRG_HOST_MASK(1000),
        eStreamWrongState      =        ERRG_HOST_MASK(1001),
        eRequestCanceled       =        ERRG_HOST_MASK(1002),

        //Gaze errors
        eInCalibrationModeError =       ERRG_HOST_MASK(1101),

        // Hands errors
        eInConfigurationError =			ERRG_HOST_MASK(1201),
        eHandsProcessing =              ERRG_HOST_MASK(1202),
        eHandsInitScene =               ERRG_HOST_MASK(1203),

        // AGC
        eIllegalAGCValue =              ERRG_HOST_MASK(1301),
        eInCalibrationProcess =         ERRG_HOST_MASK(1302),
        eClibarationError  =            ERRG_HOST_MASK(1303),
        eCalibrationTooLong =           ERRG_HOST_MASK(1304),
        eNoAGCFilter =                  ERRG_HOST_MASK(1305),
        eAutoControlError =             ERRG_HOST_MASK(1306),
        eAGCConfigFileError =           ERRG_HOST_MASK(1307),

        // Face Expressions
        eMissingConfigFiles =           ERRG_HOST_MASK(1401),
        eFailedToInitLibrary =          ERRG_HOST_MASK(1402),
        eConversionError =              ERRG_HOST_MASK(1403),
        eFrameAnalysisError =           ERRG_HOST_MASK(1404),
        eLibraryIsntInitialized =       ERRG_HOST_MASK(1405),

        // Head errors
        eHeadAlgorithmFailed =			ERRG_HOST_MASK(1501),

        // Face Recognition
        eFRInit =			            ERRG_HOST_MASK(1601),
        eFRProcess =			        ERRG_HOST_MASK(1602),
        eEnrollNotStarted =			    ERRG_HOST_MASK(1601),
        eEnrollProcess =			    ERRG_HOST_MASK(1602),
        eEnrollProcessError =           ERRG_HOST_MASK(1603),
        eEnrollDirDoesntExist =         ERRG_HOST_MASK(1604),


        // Stereo Stream Errors
        eInterleaveMissMatch =			ERRG_HOST_MASK(1701),

        // Recording/Player
        eRecordingAlreadyStarted =      ERRG_HOST_MASK(1801),
        eNoFilesToPlay =                ERRG_HOST_MASK(1802),
        eRcordingError =                ERRG_HOST_MASK(1803),

        // Read/Write camera data from flash
        eCompressionError =             ERRG_HOST_MASK(1901),
        eDecompressionError =           ERRG_HOST_MASK(1902),
        eReadFromFlash =                ERRG_HOST_MASK(1903),
        eWriteToFlash =                 ERRG_HOST_MASK(1904),
        eGetFlashSize =                 ERRG_HOST_MASK(1905),
        eFlashNotEnoughSpace =          ERRG_HOST_MASK(1906),
        eCalibrationDataLoad =          ERRG_HOST_MASK(1907),
        eWrongModelName =               ERRG_HOST_MASK(1908),
        eChmodError =                   ERRG_HOST_MASK(1909),

        // Depth Errors
        eDIModeIsNotSupported =			ERRG_HOST_MASK(2001),
        eDIModeAndVideoMismatch =		ERRG_HOST_MASK(2002),
        eDIModeAlgorithmError =			ERRG_HOST_MASK(2003),
        eDepthFromCevaConfigFileError = ERRG_HOST_MASK(2004),
        eBadPointCloudFPS =             ERRG_HOST_MASK(2005),
        eDroppedPointCloudFrame =       ERRG_HOST_MASK(2006),
		eBadPointCloudDecimationFactor=	ERRG_HOST_MASK(2007),
        eInformatioMapWithBlob =        ERRG_HOST_MASK(2008),
        eBlobNotSupported =             ERRG_HOST_MASK(2009),

        // Sensor Configuration
        eNoSensorConfiguration =		ERRG_HOST_MASK(3001),
        eNoWebCamOpticalData =          ERRG_HOST_MASK(3002),
        eFailedLoadingInuModelsDB =     ERRG_HOST_MASK(3003),
        eModelNameNotFound =            ERRG_HOST_MASK(3004),
        eGetSensorParamsError =			ERRG_HOST_MASK(3005),
        eUnexpectedNegativeBaseline =   ERRG_HOST_MASK(3006),
        eInCalibrationModeCalibrationDataNotLoaded       =   ERRG_HOST_MASK(3007),
        eInterleaveModeNotSupportedByChannel = ERRG_HOST_MASK(3008),

        // IMU
        eFailedToGetIMUHeader =		    ERRG_HOST_MASK(3101),
        eZeroIMUChannels =		        ERRG_HOST_MASK(3102),
        eFailedToSetIMUFPS =            ERRG_HOST_MASK(3103),

        // Feature Tracking
        eFTIsNotSupported =				ERRG_HOST_MASK(3201),

        //CNN
        eProcessingPaused =				ERRG_HOST_MASK(4000),
        eCnnFileOpenFailed =			ERRG_HOST_MASK(4001),
        eCnnAckReadFailed  =			ERRG_HOST_MASK(4002),
        eInitAILibraryFailed =			ERRG_HOST_MASK(4003),
        eFaceRecognitionAI =            ERRG_HOST_MASK(4004),
        eObjectsDetectionAI =           ERRG_HOST_MASK(4005),
		eCnnAttributesSize =			ERRG_HOST_MASK(4006),
		eCnnStartFailed	   =			ERRG_HOST_MASK(4007),
		eCnnStopFailed	   =			ERRG_HOST_MASK(4008),
		eCnnReleaseNetworksFailed =		ERRG_HOST_MASK(4009),
    };
}

#endif // __INUERROREXT_H__
