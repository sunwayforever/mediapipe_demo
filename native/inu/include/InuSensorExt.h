/*
* File - InuSensorExt.h
*
* This file is part of the Inuitive SDK
*
* Copyright (C) 2014 All rights reserved to Inuitive
*
*/

#ifndef __PROVIDEREXT_H__
#define __PROVIDEREXT_H__

#include "StreamsExport.h"
#include "InuSensor.h"
#include "InuDefs.h"

#include <limits>
#include <vector>

#if (defined(_MSC_VER) && (_MSC_VER <1700)) //Visual studio version before 2012
#pragma warning(disable:4482)
#endif

#ifndef MAX_PATH
#define MAX_PATH 260
#endif

namespace InuDev
{
    enum EPhaseNumber
    {
        ePhase0 = 0,
        ePhase1 = 1,
        ePhase2 = 2,
        ePhaseA = 0xA,
        ePhaseB = 0xB,
        ePhaseC = 0xC,
        ePhaseD = 0xD,
        ePhaseE = 0xE,
        ePhaseF = 0xF
    };

    enum ESensorName
    {
        eMaster = 0,
        eSlave = 1,
        eBoth = 2,
        eLeft = 3,
        eRight = 4,

    };

    ///////////////////////////////////////////////////////////////////////
    /// \brief    All Projecting Modes
    ///////////////////////////////////////////////////////////////////////
    enum EProjectorMode
    {
        eDefault = 0,
        ePatternsProjecting = 1,
        eFloodProjecting = 2,
        eAlternateProjecting = 3
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief  The type of Injection Types
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    enum EInjectionType
    {
        eInjectionNone,
        eInjectionUserDefine,
        eInjectionIAEStereo,
        eInjectionMipi,
        eInjectionDDR
    };

    struct CInjectionParams
    {
        unsigned int        mCameraWidth;
        unsigned int        mCameraHeight;
        EInjectionType      mInjectionType;

        CInjectionParams() :
            mCameraWidth(640),
            mCameraHeight(480) ,
            mInjectionType(EInjectionType::eInjectionNone)
        { }
    };

	///////////////////////////////////////////////////////////////////////
	/// \brief    Device Start params
	///////////////////////////////////////////////////////////////////////
	struct CStartDeviceParamsExt
	{
		/// \brief    The DPE parameters
		std::vector<CDpeParams> VecDpeParams;

		/// \brief    The Channel Control Params
		std::map<uint32_t, CChannelControlParams> ChannelControlParam;
	};

    struct CDeviceParamsExt : public CDeviceParams
    {
        bool                mIsSimulationMode;
        std::string         mSimulationInputFolder;

        /// \brief    if it is not empty then calibration data is loaded from this directory
        std::string         mCalibrationDirName;

        /// \brief     Injection settings
        CInjectionParams    mInjection;

        /// \brief     Projector mode settings
        EProjectorMode      mProjectionMode;

        /// \brief    If it is true than depth temperature compensation algorithm is executed
        bool                mTemperatureCompensation;
        bool                mIsCalibrationMode;

        /// \brief    Boot path for bypass mode(Calibration).
        std::string         mBootPath;

        /// \brief    The path to FW's "hardware graph"
        std::string         mHwGraphXmlPath;

        CDeviceParamsExt(const CDeviceParams& iParams=CDeviceParams()) :
            CDeviceParams(iParams),
            mIsSimulationMode(false),
            mSimulationInputFolder(),
            mCalibrationDirName(),
            mProjectionMode(ePatternsProjecting),
            mTemperatureCompensation(true),
            mIsCalibrationMode(false),
            mBootPath()
            {
                // Empty
            }
    };

    typedef unsigned char byte;


    struct CSensorRegisterParams
    {
        ESensorName mSensorNumber;
        unsigned int mAddress;
        unsigned int mData;
        unsigned int mSize;
        bool mVerify;               // "true" - verify that the load operation succeeds (performance overhead)
        std::string mFileName;
    };

    struct CSensorI2CRegisterParams
    {
        unsigned int mI2CNumber;
        unsigned int mDeviceAddress;
        unsigned int mData;
        unsigned int mDataSize;
        unsigned int mRegAddress;
        unsigned int mRegAddressSize;

        CSensorI2CRegisterParams() :
            mI2CNumber(0), mDeviceAddress(0), mData(0), mDataSize(0), mRegAddress(0), mRegAddressSize(0) {}

        CSensorI2CRegisterParams(unsigned int iI2CNumber, unsigned int iDeviceAddress,
                                 unsigned int iData, unsigned int iDataSize,
                                 unsigned int iRegAddress, unsigned int iRegAddressSize) :
            mI2CNumber(iI2CNumber), mDeviceAddress(iDeviceAddress), mData(iData), mDataSize(iDataSize), mRegAddress(iRegAddress),mRegAddressSize(iRegAddressSize) {}
    };

    struct CAgcParams
    {
        enum EIntensityAlg
        {
            eNone = 0,      // No intensity balance algorithm
            eEqual,         // Equal weight of foreground and background
            eFG,            // Prefer foreground
            eBG,            // Prefer background
        };

        enum EIntensityLevel  {  eLow = 0, eMed,  eHigh};

        enum EPowerGridRate  {  e50 = 0, e60};

        CAgcParams()
        {
            // Set defaults
            mActivated = true;
            mAutoExposure = false;
            mIntensityAlg = eEqual;
            mForgroundLevel = eHigh;
            mBackgroundLevel = eHigh;
            mAutoLumaTarget = false;
            mLumaTarget = 0x64;
            mEliminateFlickering = true;
            mPowerGridRate = e60;
            mGain[0] = mGain[1] = 0x1008;
            mShutter[0] =  mShutter[1] = 0xA7;
            mNewAlgorithm = 0;

        }

        bool            mActivated;         // True if filter is on

        // True for AE mode
        bool            mAutoExposure;

        // Auto Exposure params
        EIntensityAlg   mIntensityAlg;
        EIntensityLevel mForgroundLevel;
        EIntensityLevel mBackgroundLevel;
        bool            mAutoLumaTarget;
        byte            mLumaTarget;
        bool            mEliminateFlickering;
        EPowerGridRate  mPowerGridRate;

        // Manual Exposure params
        unsigned short  mGain[2];       // Left, Right
        unsigned int    mShutter[2];    // Left, Right

        // Use the new algorithm
        int mNewAlgorithm;
    };


    class CInjectionStream;
    class CDepthStreamExt;
    class CImuStreamExt;
	class CImageStreamExt;

    ///////////////////////////////////////////////////////////////////////
    // Class: CInuSensor
    // Role: Creation of IAF Services which provide NU3000 extended data streams
    // Inherits:
    // Responsibilities:
    //      1. Implementation of Singleton design pattern
    //      2. Knows how to generate all kinds of IAF Services
    // Comments:
    ///////////////////////////////////////////////////////////////////////
    class CInuSensorExt  : public CInuSensor
    {

    public:
        virtual ~CInuSensorExt() {}
        /// \brief    Access to the Singleton object
        INUSTREAMS_API static std::shared_ptr<CInuSensorExt>  Create(const std::string& iSensorID = std::string());

        // IAF Services
        virtual std::shared_ptr<CDepthStreamExt>	CreateDepthStreamExt(uint32_t iChannelID = DEFAULT_CHANNEL_ID) = 0;
        virtual std::shared_ptr<CImuStreamExt>		CreateImuStreamExt(uint32_t iChannelID = DEFAULT_CHANNEL_ID) = 0;
		virtual std::shared_ptr<CImageStreamExt>	CreateImageStreamExt(uint32_t iChannelID = DEFAULT_CHANNEL_ID) = 0;

  		virtual std::shared_ptr<CInjectionStream>	CreateInjectionStream(const std::string& iStreamerName = std::string()) = 0;

		using CInuSensor::Start;

		using CInuSensor::Init;

		/// \brief    Service initialization.
		///
		/// Invoked once before initialization of any other InuDev stream.
		/// After invoking Init method the sensor is still in low power consumption.
		/// This function will provide a map with all available HW channels on which a client application can receive streams of frames
		/// \param[out] oHWInfo    Developer should call the function with a new CHwInformation class and will recieved data upn the function return.
		/// \param[in] iParams    Initialized the sensor with these input parameters. It will be set to all assembled cameras.
		/// \return CInuError    Operation status which indicates on success or failure.
		virtual CInuError Init(CHwInformation& oHWInfo, const CDeviceParamsExt& iParams) = 0;

        /// \brief    Service initialization.
        ///
        /// Invoked once before initialization of any other InuDev stream.
        /// After invoking Init method the sensor is still in low power consumption.
        /// This function will provide a map with all available HW channels on which a client application can receive streams of frames
        /// \param[out] oHWInfo    Developer should call the function with a new CHwInformation class and will recieved data upn the function return.
        /// \param[out] oVecDpeParams	vector of DPE Params
        /// \param[in] iParams			Initialized the sensor with these input parameters. It will be set to all assembled cameras.
        /// \return CInuError			Operation status which indicates on success or failure.
        virtual CInuError Init(CHwInformation& oHWInfo,
            std::vector<CDpeParams>& oVecDpeParams,
            const CDeviceParamsExt& iParams = CDeviceParamsExt()) = 0;

        /// \brief    Service initialization.
        ///
        /// Invoked once before initialization of any other InuDev stream.
        /// After invoking Init method the sensor is still in low power consumption.
        /// This function will provide a map with all available HW channels on which a client application can receive streams of frames
        /// \param[out] oHWInfo    Developer should call the function with a new CHwInformation class and will recieved data upn the function return.
        /// \param[in] iParams    Initialized the sensor with these input parameters. It will be set to all assembled cameras.
        /// \param[in] iCnnParams  CNN network. The service will load the specified CNN network in parallel while InuSensor is Initialized & Startarted to reduce CNN loading time.
		/// \return CInuError    Operation status which indicates on success or failure.
        virtual CInuError Init(CHwInformation& oHWInfo, const CDeviceParamsExt& iParams, const CCnnLoadParams& iCnnParams) = 0;

		/// \brief    Start acquisition of frames.
		///
		/// Shall be invoked only after the service is successfully initialized and before any request
		/// \param[in out] iStartParams    Initialized the sensor with these ChannelsParams per channel and DPE params, and return real params from the server.
		/// \param[out] oChannelsSize    - Image size that is provided by the this sensor [Width,Height] according to channelID.
		/// \return CInuError    Operation status.
		virtual CInuError Start(std::map<uint32_t, CChannelSize>& oChannelsSize, CStartDeviceParamsExt& ioStartParams) = 0;

        //Allow setting individual camera settings.


        /// \brief Write() - Inject buffer
        ///
        /// Detailed description: Send to InuService the buffer that should be injected
        ///
        /// \param[in] iBuffer     Buffer data
        /// \param[in] iBufferLen     Buffer Length
        ///
        /// \return InuDev::CInuError     one of NU3000 Driver Errors (see in IAFExceptions.h)
        virtual CInuError       Write(const unsigned char* iBuffer, unsigned int iBufferLen) const = 0;

        /// \brief SetSensorRegister Sets a register value in a sensor
        ///
        /// \param[in]  iAddress register address
        /// \param[in]  iData new register value
        /// \param[in]  size data size
        /// \param[in]  SensorNumber sensor number numb
        /// \return CInuError
        virtual CInuError SetSensorRegister(unsigned int iAddress, unsigned int iData, unsigned int size, ESensorName SensorNumber) const  = 0;

        /// \brief GetSensorRegister gets a register value from a sensor
        ///
        /// \param[in]  iAddress register address
        /// \param[out]  iData register value
        /// \param[in]  size data size
        /// \param[in]  SensorNumber sensor number numb
        /// \return CInuError
        virtual CInuError GetSensorRegister(unsigned int iAddress, unsigned int &iData, unsigned int size, ESensorName SensorNumber) const  = 0;

        /// \brief SetRegister Sets a register value in the firmware
        /// \param[in]  iAddress register address
        /// \return CInuError
        virtual CInuError SetI2CRegister(const CSensorI2CRegisterParams& iParams) const  = 0;

        /// \brief GetRegister Gets a register value from the firmware
        /// \param[in,out]  iAddress register address
        /// \return CInuError
        virtual CInuError GetI2CRegister(CSensorI2CRegisterParams& ioParams) const  = 0;

        /// \brief Sensor Load Registers from file
        ///
        /// \param[in]  iFileName file name containing registers values
        /// \param[in]  iVerify - if it is true then the function checks that load succeeds (by reading the value of each register) and
        ///                       then the loading time is increased.
        /// \return CInuError
        virtual CInuError LoadSensorRegistersFromFile(const std::string &iFileName, bool iVerify) const  = 0;

        virtual CInuError LoadRegistersFromFile(const std::string &iFileName, bool iVerify) const = 0;



        /// \brief    Define parameters for AGC (data is sent to chip).
        ///
        /// \param[in,out] ioParams   Input structure that may be changed by the service in order to make them coherent.
        ///
        /// \return CInuError    eIllegalAGCValue.
        virtual CInuError       SetAgcParams(CAgcParams& ioParams) const = 0;

        /// \brief    Get parameters that are currently used by AGC.
        ///
        /// \param[in] oParams    Output parameters.
        virtual CInuError       GetAgcParams(CAgcParams& oParams) const = 0;

        /// \brief    Start manual calibration process.
        virtual CInuError       ManualCalibration() const = 0;

        /// \brief    Report on calibration progress.
        ///
        /// \return CInuError    eSatusOK, eInCalibrationProcess, eClibarationError.
        virtual CInuError       ManualCalibrationStatus() const = 0;

        /// \brief    Start tuning sensor register to improve image quality
        ///
        /// \return CInuError    eSatusOK
        virtual CInuError       StartTuning() const = 0;


        virtual CInuError SetPhaseRegister(unsigned int iAddress, unsigned int iData, EPhaseNumber iPhase) const = 0;
        virtual CInuError SetSensorPhaseRegister(unsigned int iAddress, unsigned int iData, EPhaseNumber iPhase, unsigned int iSize, ESensorName iSensorNumber) const = 0;

        virtual CInuError ActivatePhase() const = 0;
        virtual CInuError ClearPhase() const = 0;


        /// \brief SetRegister Sets a register value in the firmware
        /// \param[in]  iAddress register address
        /// \param[in]  iData new register value
        /// \return CInuError
        virtual CInuError SetRegister(unsigned int iAddress, unsigned int iData) const  = 0;

        /// \brief GetRegister Gets a register value from the firmware
        /// \param[in,out]  iAddress register address
        /// \param[out]  iData register value
        /// \return CInuError
        virtual CInuError GetRegister(unsigned int iAddress, unsigned int &iData) const  = 0;

        /// \brief Load Registers from file
        /// \param[in]  iFileName file name containing registers values
        /// \param[in]  iVerify - if it is true then the function checks that load succeeds (by reading the value of each register) and
        ///                       then the loading time is increased.
        /// \return CInuError
        virtual CInuError LoadControlRegistersFromFile(const std::string &iFileName, bool iVerify) const = 0;

		/// \brief    Reload persistent configurable parameters
        /// \param[in] iClient    reload for client (IAFClient).
        /// \param[in] iServer    reload for server (IAFServer).
        /// \return CInuError
        virtual CInuError ReloadConfiguration(bool iClient, bool iServer) = 0;

        /// \brief    Ability to override the default boot mode.
        /// \param[in]  iMode 0- boot from file, 1- boot from flash
        /// \param[in]  iPassword - this operation is protected and can't be executed without providing the correct password
        /// \return CInuError
        virtual CInuError SetBootMode(bool iMode, const std::string & iPassword) = 0;

        /// \brief   Performs a memory scan in the target.
        /// \return CInuError
        virtual CInuError PerformMemoryScan(unsigned int& iMemoryScanResult) = 0;

        /// \brief   Reads a Memory buffer from the target.
        ///
        /// Reads a Memory buffer from the target and saves it to the file name.
        ///
        /// \param[in,out] iStartAddress    Initial read address.
        /// \param[in,out] iSize            Buffer size to read.
        /// \param[in,out] iFileName        File that will store the read data.
        /// \return CInuError
        virtual CInuError ReadMemoryBuffer(unsigned int iStartAddress, unsigned int iSize, std::string iFileName)  = 0;

        /// \brief    Writes a Memory buffer to the target.
        ///
        /// Sends a buffer that is contained in the file to the target.
        /// \param[in,out] iStartAddress    Initial write address.
        /// \param[in,out] iFileName        File that contains the buffer to send.
        /// \return CInuError
        virtual CInuError WriteMemoryBuffer(unsigned int iStartAddress, std::string iFileName)  = 0;


        /// \brief    Show (in log file) statistics data which is collected by the FW
        /// \return CInuError
        virtual CInuError ShowStatistics() = 0;


        /// \brief    Returns extended state of CBaseStream which is special to CInuSensorExt object
        virtual ESensorState GetState() const = 0;


        /// \brief    Write buffer to General Purpose service.
        ///
        /// This function is used to "Control" FDK function node.
        /// The caller application should know how to pack the data so it will be successfully interpreted by the service.
        /// \param[in] iBuffer    Transfered data.
        /// \param[in,out] iBufferLen    Transferred buffer length.
        /// \param[in,out] iFdkNodeName  Transferred FDK node name.
        /// \return InuDev::CInuError     Error code, InDev::eOK if operation successfully completed.
        virtual CInuError ControlFDK(const unsigned char* iControlCommandBuffer, uint32_t iControlCommandBufferLen, const std::string& iFdkNodeName) const = 0;


		/// \brief    SetInjectResolution
		///
		/// Should be called only after the sensor had initialized and before it was started.
		/// \param[in] iStreamerName The Streamer Name on which the write operation will affect.
		/// \param[in] iChannelSize The size of the actual data inside the input image after writing.
		/// \return CInuError    Error code, InDev::eOK if operation successfully completed.
		virtual CInuError SetInjectResolution(const std::string& iStreamerName,	const CChannelSize& iChannelSize) = 0;


        /// \brief    ConfigurePWM
        ///
        /// Should be called only after the sensor had initialized and before it was started.
        /// \param[in] iPwmId the pwm component id.
        /// \param[in] iTriggersPerSecond number of triggers per second.
        /// \param[in] iTriggerLengthUs the trigger length in microseconds.
        /// \return CInuError    Error code, InDev::eOK if operation successfully completed.
        virtual CInuError ConfigurePWM(uint32_t iPwmId, uint32_t iTriggersPerSecond, uint32_t iTriggerLengthUs) = 0;

        /// \brief    StartPWM
        ///
        /// Should be called only after the sensor had started.
        /// \param[in] iPwmId the pwm component id.
        /// \return CInuError    Error code, InDev::eOK if operation successfully completed.
        virtual CInuError StartPWM(uint32_t iPwmId) = 0;

        /// \brief    StopPWM
        ///
        /// Should be called only after the sensor had started.
        /// \param[in] iPwmId the pwm component id.
        /// \return CInuError    Error code, InDev::eOK if operation successfully completed.
        virtual CInuError StopPWM(uint32_t iPwmId) = 0;


    protected:
        CInuSensorExt() {}
    };

}

#endif // __PROVIDEREXT_H__
