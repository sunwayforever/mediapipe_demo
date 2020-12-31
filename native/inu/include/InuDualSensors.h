/*
* File - InuDualSensors.h
*
* This file is part of the Inuitive SDK
*
* Copyright (C) 2017 All rights reserved to Inuitive
*
*/

#ifndef __INUDUALSENSORS_H__
#define __INUDUALSENSORS_H__

#ifdef INUDUALSENSORS_EXPORTS
#ifdef __GNUC__
#ifdef __linux__
#define INUDUALSENSORS_API __attribute__((visibility("default")))
#else
#define INUDUALSENSORS_API __attribute__ ((visibility ("default")))
#endif
#else
#define INUDUALSENSORS_API __declspec(dllexport)
#endif
#define INUDUALSENSORS_API_TEMPLATE
#else
#ifdef __GNUC__
#ifdef __linux__
#define INUDUALSENSORS_API
#else
#define INUDUALSENSORS_API __attribute__ ((visibility ("default")))
#endif
#else
#define INUDUALSENSORS_API __declspec(dllimport)
#endif
#define INUDUALSENSORS_API_TEMPLATE extern
#endif

#include "InuSensor.h"
#include "ImuStream.h"

#include <map>
#include <mutex>
#include <atomic>

namespace InuDev
{
    // Forward declarations from InuDev
    class CImageFrame;
    class CDepthStream;
    class CStereoImageFrame;
    class CStereoImageStream;

    ////////////////////////////////////////////////////////////////////////
    /// \brief    CInuDualSensors represents Dual Inuitive Sensors, master and slave configuration. 
    ///
    /// Responsibilities: 
    ///      1. Initialization and termination of both sensors in the right order. 
    ///      2. Acquisition of synchronized depth frames
    ///      3. Acquisition of Master IMU frames
    ///
    ////////////////////////////////////////////////////////////////////////
    class INUDUALSENSORS_API CInuDualSensors
    {
        
    public:

        CInuDualSensors();

        virtual ~CInuDualSensors();

        /// \brief	Prototype of Disconnection events callback function.
        typedef std::function<void()> CallbackDisconnection;

        /// \brief	Initializes both InuSensors 
        /// \param[in] iFPS    set frames per second - default is 60.0
        CInuError Init(CallbackDisconnection iDisconnnectionCallback, float iFPS = 60.0, ESensorResolution iReslution = eDefaultResolution);

        /// \brief	Starts both InuSensors and prepares them for work.
        CInuError Start();

        /// \brief	Stops the sensors, should be called if Start was invoked 
        CInuError Stop();

        /// \brief	Closes both InuSensors, should be called if Init was invoked 
        CInuError Terminate();

        /// \brief	Prototype of Depth callback function.
        typedef std::function<void(std::shared_ptr<const CImageFrame> iMasterFrame, std::shared_ptr<const CImageFrame> iSlaveFrame, CInuError iError)> CallbackFunctionDepth;
        
        /// \brief	Starts depth services of both InuSensors which provide synchronized depth frames 
        /// \param[in] iDepthCallback    Depth callback function that is invoked when frames with the same index are received from both InuSensors.
        CInuError StartDepth(CallbackFunctionDepth iDepthCallback);

        /// \brief	Stops the Depth service of both InuSensors 
        CInuError StopDepth();

        /// \brief	Prototype of Depth callback function.
        typedef std::function<void(std::shared_ptr<const CStereoImageFrame> iMasterFrame, std::shared_ptr<const CStereoImageFrame> iSlaveFrame, CInuError iError)> CallbackFunctionStereoImage;

        /// \brief	Starts StereoImage services of both InuSensors which provide synchronized video frames 
        /// \param[in] iStereoImageCallback    Depth callback function that is invoked when frames with the same index are received from both InuSensors.
        CInuError StartStereoImage(CallbackFunctionStereoImage iStereoImageCallback);

        /// \brief	Stops the Depth service of both InuSensors 
        CInuError StopStereoImage();

        /// \brief	Starts IMU service of Master InuSensor. It provides IMU frames from the master InuSensor. 
        /// \param[in] iIMUCallback    IMU callback function that is invoked when IMU frame is received from master InuSensor.
        CInuError StartIMU(CImuStream::CallbackFunction iIMUCallback);

        /// \brief	Stops IMU service 
        CInuError StopIMU();

        /// \brief	Retrieves IMU temperature 
        CInuError GetIMUTemprature(float& oMasterTemp);
 
        /// \brief	InuSensor type: Master or Slave
        enum ESensorType
        {
            eMaster,
            eSlave,
        };

        /// \brief	All possible open InuStreams that are in use.
        enum EStreamType
        {
            eInuSensor,
            eDepthStream,
            eStereoImageStream,
            eImuStream
        };

        /// \brief	Get the stream interfaces for the master or the salve
        std::shared_ptr<CBaseStream> GetStream(ESensorType sensorType, EStreamType streamType);

    private:
        
        /// \brief	Maximum number of unsynchronized frames that are held until match frame is received 
        static const int MAX_FRAMES_TO_HOLD = 20;



#ifdef _MSC_VER
#pragma warning(disable: 4251)
#endif

        /// \brief	Client callback function which is invoked when one of the sensors is disconnected.
        CallbackDisconnection mCallbackDisconnection;

        /// \brief	Client callback function which is invoked when synchronized depth frames are received from both InuSensors.
        CallbackFunctionDepth mCallbackDepth;

        /// \brief	Client callback function which is invoked when synchronized StereoImage frames are received from both InuSensors.
        CallbackFunctionStereoImage mCallbackStereoImage;

        /// \brief	Collection of all open Master services.
        std::map<EStreamType, std::shared_ptr<CBaseStream>> mMasterStreams;

        /// \brief	master sensor object.
        std::shared_ptr<InuDev::CInuSensor> mMasterSensor;

        /// \brief	Collection of all open Slave services.
        std::map<EStreamType, std::shared_ptr<CBaseStream>> mSlaveStreams;

        /// \brief	slave sensor object.
        std::shared_ptr<InuDev::CInuSensor> mSlaveSensor;


        /// \brief	Collection of all depth frames that were received from Master and has not been delivered to client yet.
        std::map<unsigned int, std::shared_ptr<const CImageFrame>> mMasterDepthFrames;

        /// \brief	Collection of all depth frames that were received from Slave and has not been delivered to client yet.
        std::map<unsigned int, std::shared_ptr<const CImageFrame>> mSlaveDepthFrames;

        /// \brief	Collection of all StereoImage frames that were received from Master and has not been delivered to client yet.
        std::map<unsigned int, std::shared_ptr<const CStereoImageFrame>> mMasterStereoImageFrames;

        /// \brief	Collection of all StereoImage frames that were received from Slave and has not been delivered to client yet.
        std::map<unsigned int, std::shared_ptr<const CStereoImageFrame>> mSlaveStereoImageFrames;

        /// \brief	This synchronization object is used to prevent changes in the object state concurrently.
        std::atomic<bool> mConnetionCallbackLock;

        /// \brief	This synchronization object is used to protect concurrent execution of connection events handler.
        std::mutex mCollectionSynchronizer;

#ifdef _MSC_VER
#pragma warning(default: 4251)
#endif

        /// \brief	 Initialization of specific InuService (we don't know if the master or slave is connected to it)
        /// \param[in] iServiceID    Service to initialize.
        /// \param[in] iFPS    set frames per second - default is 60.0
        CInuError Init(const std::string& iServiceID, float iFPS, ESensorResolution iReslution);

        /// \brief	 Start of one depth service 
        /// \param[in] iInuSensor    Master/Slave sensor.
        /// \param[out] oStreamsCollection    Which collection to add the created stream .
        CInuError StartDepth(std::shared_ptr<CInuSensor> iInuSensor, std::map<EStreamType, std::shared_ptr<CBaseStream>>& oStreamsCollection);

        /// \brief	 Start of one StereoImage service 
        /// \param[in] iInuSensor    Master/Slave sensor.
        /// \param[out] oStreamsCollection    Which collection to add the created stream .
        CInuError StartStereoImage(std::shared_ptr<CInuSensor> iInuSensor, std::map<EStreamType, std::shared_ptr<CBaseStream>>& oStreamsCollection);

        /// \brief	 Stop of one stream service 
        /// \param[out] oStreamsCollection    Which collection to delete the depth/StereoImage stream .
        /// \param[in] iType    type of stream to stop.
        CInuError StopStream(std::map<EStreamType, std::shared_ptr<CBaseStream>>& oStreamsCollection, EStreamType iType);

        /// \brief	 Checks if Master or Slave is connected to input InuSensor object
        /// \param[in] iSensor    Input InuSensor.
        ESensorType ReadSensorType(std::shared_ptr<CInuSensor> iSensor); 

        /// \brief	 Callback that is invoked when connection event is received from one of the InuSensors
        /// \param[in] iStream    The specific InuSensor object that raised the connection event.
        /// \param[in] iNewState    New connection state.
        /// \param[in] iErrorCode    Error code (in case the callback could not be invoked as expected).
        void ConnectionEventCallback(std::shared_ptr<CInuSensor> iStream, EConnectionState iNewState, CInuError iErrorCode);

        /// \brief	 Callback that is invoked when depth frame is received from one of the InuSensors
        /// \param[in] iStream    The specific InuSensor object that raised the connection event.
        /// \param[in] iFrame    Acquired frame.
        /// \param[in] iErrorCode    Error code (in case the callback could not be invoked as expected).
        void DepthEventCallback(std::shared_ptr<CDepthStream> iStream, std::shared_ptr<const CImageFrame> iFrame, CInuError iErrorCode);

        /// \brief	 Callback that is invoked when StereoImage frame is received from one of the InuSensors
        /// \param[in] iStream    The specific InuSensor object that raised the connection event.
        /// \param[in] iFrame    Acquired frame.
        /// \param[in] iErrorCode    Error code (in case the callback could not be invoked as expected).
        void StereoImageEventCallback(std::shared_ptr<CStereoImageStream> iStream, std::shared_ptr<const CStereoImageFrame> iFrame, CInuError iErrorCode);

        /// \brief	   Synchronized frames collection with input acquired frame
        /// \param[in] iInputFrame    Last acquired frame from one InuSensor
        /// \param[in, out] ioCurrentStreamCollection    Frames collection of current stream (Master or Slave)
        /// \param[in, out] ioOtherCollection    Frames collection other than current stream  (Master or Slave)
        template <class TFrame, class TCallback>
        CInuError SynchronizeAcquiredFrames(std::shared_ptr<const TFrame> iInputFrame,
            std::map<unsigned int, std::shared_ptr<const TFrame>>& ioCurrentStreamCollection,
            std::map<unsigned int, std::shared_ptr<const TFrame>>& ioOtherCollection,
            std::map<unsigned int, std::shared_ptr<const TFrame>>& iMasterFramesCollection,
            std::map<unsigned int, std::shared_ptr<const TFrame>>& iSlaveFramesCollection,
            TCallback iCallback);

    };

}

#endif
