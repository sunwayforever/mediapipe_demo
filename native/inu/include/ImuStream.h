/*
* File - ImuStream.h
*
* This file is part of the Inuitive SDK
*
* Copyright (C) 2014 All rights reserved to Inuitive
*
*/


#ifndef __IMUSTREAM_H__
#define __IMUSTREAM_H__
#include <map>
#include "BaseOutputStream.h"
#include "GeneralFrame.h"

#include <functional>

namespace InuDev
{
    ////////////////////////////////////////////////////////////////////////
    /// \brief  Represents the type of the sensor which CImuData refer to.
    ////////////////////////////////////////////////////////////////////////

    /// \brief    IMU  types.
    enum EImuType
    {
        eAccelerometer,
        eGyroscope,
        eMagnetometer,
        eNumOfImuTypes,
    };

    /// \brief    All possible data objects that are provide by IMU device.
    enum EPosSensorChannelType
    {
        eAccelerometerX,
        eAccelerometerY,
        eAccelerometerZ,
        eGyroscopeX,
        eGyroscopeY,
        eGyroscopeZ,
        eMagnetometerX,
        eMagnetometerY,
        eMagnetometerZ,
        eTimeStamp,
        eInvalidChannelType = 1000
    };

    /// \brief    Represents collection of IMU data that is provided by IMU device.
    struct CImuFrame : public CBaseFrame
    {
        std::map<EImuType, CPoint3D> SensorsData;

        /// \brief  Index of the same IMU type, i.e. continuous index of all IMU frames of the same type.
        uint32_t SubIndexFrame;

        /// \brief  Current temperature of IMU in Celsius deg. std::numeric_limits<float>::max() if temperature is not available
        float Temperature;

        bool GetSensorData(EImuType etype, CPoint3D& point)  const
        {
            if (SensorsData.find(etype) != SensorsData.end())
            {
                point = SensorsData.at(etype);
                return true;
            }
            return false;
        }

        CImuFrame()
            :	Temperature(std::numeric_limits<float>::max())
        {}
    };

    /// \brief    RAW IMU data in IIO format. It is possible to retrieve IMU data in this format.
    struct CIioChannelInfo
   {
       float Scale;
       float Offset;
       uint32_t Bytes;
       uint32_t BitsUsed;
       uint32_t Shift;
       uint64_t Mask;
       uint32_t Be;
       uint32_t IsSigned;
       uint32_t Location;
       EPosSensorChannelType ChannelType;
     };

    ////////////////////////////////////////////////////////////////////////
    /// \brief   Interface for IMU service.
    ///
    /// Role: Controls IMU streaming service and provides general or IMU frames.
    ///       IMU frames are provided only if the connected device supports IMU HW components.
    ///       The caller application should be familiar with provided frames and should know how to interpret them.
    ///
    /// Responsibilities:
    ///      1. Derives CBaseStream interface
    ///      2. Knows how to acquire one general frame (pull)
    ///      3. Knows how to provide a continuous stream of general frames (push)
    ///
    ////////////////////////////////////////////////////////////////////////
    class CImuStream : public CBaseOutputStream
    {
    public:

        /// \brief    Prototype of callback function which is used by the Register method (frames in IIO format).
        ///
        /// This function is invoked any time a frame is ready, or if an error occurs. The parameters of this function are:
        /// Caller stream object, received general frame and result code.
        //typedef std::function<void(std::shared_ptr<CImuStream> , std::shared_ptr<const CGeneralFrame>,  CInuError)> CallbackFunction;

        /// \brief    Prototype of callback function which is used by the Register method (IMU Frames).
        ///
        /// This function is invoked any time a frame is ready, or if an error occurs. The parameters of this function are:
        /// Caller stream object, received IMU frame and result code.
        typedef std::function<void(std::shared_ptr<CImuStream> , std::shared_ptr<const CImuFrame>,  CInuError)>     CallbackFunction;



        virtual ~CImuStream() {}

        /// \brief    Retrieves CImuFrame frame (pull)
        ///
        /// This method returns when a new CImuFrame is ready (blocking) or if an input timeout has elapsed.
        /// It shall be called only after a Start() was is invoked and but before any invocation of a Stop() is invoked.
        /// \param[out] oImuFrame    Array of CImuData sensor values.
        /// \param[in] iTimeout    Function is returned if timeout has elapsed even if no new frame is ready.
        /// \return CInuError    Error code, InDev::eOK if operation successfully completed.
        virtual CInuError GetFrame(std::shared_ptr<const CImuFrame>& oImuFrame, unsigned int iTimeout = FPS_BASED_TIMEOUT) = 0;

        /// \brief    Registration/Unregistration for receiving stream of general frames (push)
        ///
        /// The provided callback function is called when a new frame is ready (non-blocking).
        /// It shall be called only after a Start() was is invoked and but before any invocation of a Stop() is invoked.
        /// \param[in] iCallback    Callback function which is invoked when a new frame is ready.
        ///                         Send nullptr to unregister for receiving frames.
        /// \param[in] iTimeout    Callback is invoked if a timeout has elapsed even if no new frame is ready.
        /// \return CInuError    Error code, InDev::eOK if operation successfully completed.
        virtual CInuError Register(CallbackFunction iCallback, unsigned int iTimeout = FPS_BASED_TIMEOUT) = 0;

        /// \brief    Get the IMU sensor temperature
        /// \param[out] oTemperature    returns the temperature in Celsius .
        /// \return CInuError    Error code, InDev::eOK if operation successfully completed.
        virtual CInuError GetTemperature(float &oTemperature) = 0;

        /// \brief    Set IMU params
        /// \param[in] iParams IMU params to be set.
        /// \return CInuError    Error code, InDev::eOK if operation successfully completed.
        virtual CInuError SetImuParams(const std::map<EImuType, std::pair<float, float>>& iParams) = 0;

        /// \brief    Get IMU params
        /// \param[out] oParams   IMU params that are currently used
        /// \return CInuError    Error code, InDev::eOK if operation successfully completed.
        virtual CInuError GetImuParams(std::map<EImuType, std::pair<float, float>>& oParams)  = 0;

    };
}

#endif
