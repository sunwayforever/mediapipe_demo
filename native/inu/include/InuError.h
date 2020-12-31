/*
* File - InuError.h
*
* This file is part of the Inuitive SDK
*
* Copyright (C) 2014 All rights reserved to Inuitive  
*
*/


#ifndef __INUERROR_H__
#define __INUERROR_H__

#ifdef INUSTREAMS_EXPORTS
	#ifdef __GNUC__
		#ifndef __linux__
			#define INUSTREAMS_ERROR_API __attribute__ ((dllexport))
		#else
			#define INUSTREAMS_ERROR_API __attribute__ ((visibility ("default")))
		#endif
	#else
		#define INUSTREAMS_ERROR_API __declspec(dllexport)
	#endif
#else
	#ifdef __GNUC__
		#ifndef __linux__
			#define INUSTREAMS_ERROR_API __attribute__ ((dllimport))
		#else
			#define INUSTREAMS_ERROR_API __attribute__ ((visibility ("default")))
		#endif
	#else
		#define INUSTREAMS_ERROR_API
	#endif
#endif

#include <string>

namespace InuDev
{
    ///////////////////////////////////////////////////////////////////////
    // \brief    Error codes that may be returned by InuDev
    ///////////////////////////////////////////////////////////////////////
    enum EErrorCode {
        eOK = 0,                       
        eInternalError = 1,             ///<  Internal error - unexpected error  
        eInitError = 2,                 ///<  Fails to initiate service 
        eTimeoutError = 3,              ///<  Request has failed due to time out 
        eStateError = 4,                ///<  Requested operation is prohibited for current state of this Stream
        eOperationCanceled = 5,         ///<  A stop command canceled the operation
        eUnexpectedParameterValue = 6,  ///<  Illegal input parameter
        eDualSensorIdentificationError = 7,  ///<  Sensor Identification Error, please check Master/Slave configuration
        eInvalidFrame = 8,              ///<  Invalid frame was received from InuService


		eServiceProcessFailure = 11,    ///<  InuService isn't running. CInuSensor object must be restarted in order to reconnect with InuService.
		eSensorDetectionFailure = 12,	///<  No sensor is detected. Recheck that the Sensor is connected to USB port and then restart CInuSensor object.

        eInCalibrationMode = 20,        ///<  The service is in calibration mode 

        eRecordError = 30,              ///<  Record input/output failed
        eFailedSensorControlParams = 31,///<  Sensor control patially successful

		eNumberOfErrorCodes
    };

    ////////////////////////////////////////////////////////////////////////
    /// \brief    InuDev Error class. 
    ///
    /// Role: Encapsulates internal error presentation and provides comprehensive error codes.
    ///
    /// Responsibilities: 
    //      1. Knows how to generate a meaning error description (string or EErrorCode)
    ///
    ////////////////////////////////////////////////////////////////////////
    class INUSTREAMS_ERROR_API CInuError
    {
    public:

        /// \brief    Construction from InuDev internal error code.
        CInuError(int errorCode);

        /// \brief    Construction from InuDev internal error code and comprehensive error description. 
        CInuError(int errorCode, std::string errorString);

        CInuError(int errorCode, const char* logFormatString , ...); // Additional Log information (if there are more actual parameters then it should be a formatted string

        /// \brief    Construction from InuDev exported error code.
        explicit CInuError(EErrorCode errorCode);

         ~CInuError();

         /// \brief    Conversion to exported error code.
        operator EErrorCode()   const;

        /// \brief    Returns InuDev internal error code.
        operator int()          const  {  return mErrorCode; }

        /// \brief    Conversion to comprehensive error description.
        operator std::string()  const;

    private:

        /// \brief    InuDev internal error code.
        int mErrorCode;

        /// \brief    Comprehensive error description.
#ifdef _MSC_VER
        #pragma warning(suppress: 4251)
        std::string mErrorString;
#else
        std::string mErrorString;
#endif
    };
}

#endif  //__INUERROR_H__
