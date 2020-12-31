/*
 * File - InuSensor.h
 *
 * This file is part of the Inuitive SDK
 *
 * Copyright (C) 2014 All rights reserved to Inuitive
 *
 */

#ifndef __INUSENSOR_H__
#define __INUSENSOR_H__

#include "StreamsExport.h"
#include "InuError.h"
#include "BaseOutputStream.h"
#include "InuDefs.h"
#include "CalibrationData.h"
#include "HwInformation.h"

#include <map>
#include <functional>
#include <stdint.h>

namespace InuDev
{
    // Forward decelerations
    class CDepthStream;
    class CStereoImageStream;
    class CImageStream;
    class CImuStream;
    class CUserDefinedStream;
    class CAudioStream;
    class CFeaturesTrackingStream;
    class CSlamStream;
    class CObjectsDetectionStream;
    class CCnnStream;
    class CFaceRecognitionStream;
    class CPointCloudStream;
    class CInjectionStream;
    class CCnnAppStream;
	class CHistogramStream;
    class CHwInformation;
    class CVoxelsStream;

    ///////////////////////////////////////////////////////////////////////
    /// \brief    CNN network parameters
    ///////////////////////////////////////////////////////////////////////
    struct CCnnLoadParams
    {
        //////////////////////////////////////////////////////
        /// \brief    Defines the types od Cnn network engine
        //////////////////////////////////////////////////////
        enum ECnnNetworkEngineType
        {
            eCeva,
            eSynopsys
        };

        //////////////////////////////////////////////////////
        /// \brief    Defines Cnn input fraction bits
        //////////////////////////////////////////////////////
        enum ECnnInputImageRawScale
        {
            CNN_INPUT_IMAGE_RAW_SCALE_1,    ///< Sets the input fraction bits to 8
            CNN_INPUT_IMAGE_RAW_SCALE_256   ///< Sets the input fraction bits to 0
        };

        /////////////////////////////////////////////////////////////////////////////////
        /// \brief  Decides whether or not to swap the channel order of the input image
        /////////////////////////////////////////////////////////////////////////////////
        enum ECnnChannelSwap
        {
            CNN_NETWORK_CHANNEL_NO_SWAP,
            CNN_NETWORK_CHANNEL_SWAP
        };

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief  Choose network output format between fixed-point 16 bit element to floating-point 32 bit
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        enum ECnnResultsFormat
        {
            CNN_FIXED_POINT_QUERY,
            CNN_FLOATING_POINT_QUERY
        };

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief  The type of CNN network
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        enum ECnnNetType
        {
            CNN_OTHER_CNN,
            CNN_YOLO_CNN
        };

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief  Network output format between fixed-point 16 bit element to floating-point 32 bit
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        enum ECNNBitAccuracy
        {
            CNN_BIT_ACCURACY_8BIT,
            CNN_BIT_ACCURACY_16BIT,
            CNN_BIT_ACCURACY_INVALID
        };

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief  optional special pre/post processing for the images that are processed by the loaded CNN should be set to "CCnnLoadParams::ProcesssingFlags"
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        enum ECnnProcessingFlags
        {
            CNN_PROC_FLAGS_NONE                   = 0,
            // Performs the yoloV3 post-process on the arc processor within the module. The output is a list of bounding Boxes.
            // The first 4 bytes in the output blob contains number of boxes (int format), and the boxes of type BBOX follows.
            CNN_PROC_FLAGS_YOLO_V3_POSTPROCESS    = 1,
            // Convert 12 bits to RGB 24 bits input  format to the CNN
            CNN_PROC_FLAGS_CONVERT_12_BITS_TO_888 = 2,
            //  Convert 12 bits to 8 bits grayscale input  format to the CNN
            CNN_PROC_FLAGS_CONVERT_12_BITS_TO_8   = 4,
            //  Convert RGB 24 bits to 8 bits grayscale input  format to the CNN
            CNN_PROC_FLAGS_CONVERT_888_BITS_TO_8  = 8
        };

        enum ECnnVersion
        {
            CNN_VERSION_2017_0 = 20170,
            CNN_VERSION_2017_ED1 = 20171,
            CNN_VERSION_2017_ED2 = 20172,
            CNN_VERSION_2018_0 = 20180,
            CNN_VERSION_2018_3 = 20183,
            CNN_CURRENT_VERSION = CNN_VERSION_2018_3
        };

        /// \brief	Network unique ID
        std::string NetworkID;

        /// \brief	Network is loaded from this file.
        std::string NetworkFilename;

        /// \brief The cnn network attributes
        ECnnNetworkEngineType   CnnNetworkEngineType;
        ECnnInputImageRawScale  CnnInputImageRawScale;
        ECnnChannelSwap         CnnChannelSwap;
        ECnnResultsFormat       CnnResultsFormat;
        ECnnNetType             CnnNetType;
        ECNNBitAccuracy         BitAccuracy;
        uint32_t                DdrWorkspaceSize;
        uint32_t                InternalWorkspaceSize;
        ECnnVersion             Version;
        uint32_t                PipeDepth;
        uint32_t                ProcesssingFlags;

        CCnnLoadParams() :
            NetworkID(),
            NetworkFilename(),
            CnnNetworkEngineType(eSynopsys),
            CnnInputImageRawScale(CNN_INPUT_IMAGE_RAW_SCALE_1),
            CnnChannelSwap(CNN_NETWORK_CHANNEL_NO_SWAP),
            CnnResultsFormat(CNN_FIXED_POINT_QUERY),
            CnnNetType(CNN_OTHER_CNN),
            BitAccuracy(CNN_BIT_ACCURACY_16BIT),
            DdrWorkspaceSize(70000000),
            InternalWorkspaceSize(100000),
            Version(CNN_VERSION_2018_3),
            PipeDepth(1),
            ProcesssingFlags(CNN_PROC_FLAGS_NONE)
        {}
    };

    ///////////////////////////////////////////////////////////////////////
    /// \brief    Information about SW and HW versions
    ///////////////////////////////////////////////////////////////////////
    struct CEntityVersion
    {
        /// \brief    Each version entity has a unique identification number
        enum EEntitiesID
        {
            eSerialNumber,			///<  Device Serial Number
            eModelNumber,			///<  Device Model Number
            ePartNumber,			///<  Device Part Number
            eRevisionNumber,		///<  Device Revision Number
            eMasterID,				///<  Master sensor ID
            eSlaveID,				///<  Slave sensor ID
            eFWVersion,				///<  Firmware
            eHWVersion,				///<  Hardware version, returned values are defined by EHWVersion enumerator
            eStreams,				///<  InuStreams.dll
            eServices,				///<  InuService.exe
            eHWRevision,			///<  Hardware revision (0x3000 for NU3000 or 0x4000 for NU4000)
            eLastBurnTime,          ///<  Time of the last burn of flash
            eBootfixTimestamp,		///<  Boot fix time stamp
            eBootfixVersion,		///<  Boot fix Version
            eBootID,				///<  ID of boot that is in use
            eDualSensorsMaster,		///<  Indication if this is the Master of Dual Sensors configuration
            eISPVersion,            ///<  ISP version
            eCalibrationTemperature,///<  Temperature of current calibration that is in use (or std::numeric_limits<int>::max() if temperature isn't defined)
            eTriggerMode,           ///<  True if system is working in trigger mode where the image acquisition is done according to external trigger
            eCalibrationVersion,    ///<  InuCalibration version which was used to generate the calibration data
            eUSBSpeed               ///<  USB Speed (0-Unknown, 1-USB1, 2-USB2, and so forth)
        };

        /// \brief    Version Entity ID
        EEntitiesID ID;

        /// \brief    Name of this entity
        std::string Name;

        /// \brief    Version number
        unsigned int VersionNum;

        /// \brief    Version description
        std::string VersionName;
    };

    ///////////////////////////////////////////////////////////////////////
    /// \brief    HW revisions (provided by CEntityVersion::eHWRevision)
    ///////////////////////////////////////////////////////////////////////

    enum EHWVersion
    {
        eA0  = 0,          ///<  First chip generation
        eB0 = 1,           ///<  Second chip generation
        eUnknown
    };

    ///////////////////////////////////////////////////////////////////////
    /// \brief    All types of cameras that can be assembled in InuSensor
    ///////////////////////////////////////////////////////////////////////
    enum ECameraName
    {
        eVideo = 0,				///<  Stereo camera which is used for Depth calculation
        eWebCam = 2,			///<  RGB or Fisheye camera
        eAllCameras = 100       ///<  limited to 255
    };


    ///////////////////////////////////////////////////////////////////////
    /// \brief    All possible states of connection with Sensor
    ///////////////////////////////////////////////////////////////////////
    enum EConnectionState
    {
        eUnknownConnectionState = 0,	///<  Initial state, connection has not been established
        eConnected = 1,					///<  Sensor is connected
        eDisconnected = 5,				///<  No Sensor is connected
        eServiceDisconnected = 4,		///<  Can't communicate with InuService
    };

    ///////////////////////////////////////////////////////////////////////
    /// \brief    Image size that is provided by the this sensor [Width,Height] according to channelID.
    ///////////////////////////////////////////////////////////////////////
    typedef CPoint < 2, uint32_t> CChannelSize;

    ///////////////////////////////////////////////////////////////////////
    /// \brief    CDpeParams parameters
    ///////////////////////////////////////////////////////////////////////
    struct CDpeParams
    {
        std::string fileName;
        uint32_t frameNum;

        CDpeParams(std::string iFileName = "", uint32_t iFrameNum = 1)
            : fileName(iFileName),
              frameNum(iFrameNum)
        {}
    };

    ///////////////////////////////////////////////////////////////////////
    /// \brief    Sensor parameters
    ///////////////////////////////////////////////////////////////////////
    struct CDeviceParams : public CChannelControlParams
    {
        /// \brief    The path to FW's "software graph"
        std::string GraphXmlPath;

        /// \brief    The DPE parameters
        std::vector<CDpeParams> VecDpeParams;//DEPRICATED

        /// \brief    Default constructor which defines the default FPS and resolution
        CDeviceParams(ESensorResolution iResolution = eDefaultResolution, uint32_t iFPS = (uint32_t)USE_DEFAULT_FPS)
            : CChannelControlParams(iResolution, iFPS)
        {}
    };

    /// \brief    Exposure parameters
    ///////////////////////////////////////////////////////////////////////
    struct CExposureParams
    {
        /// \brief    Exposure Time of the sensor
        uint32_t    ExposureTime;

        /// \brief    Digital Gain of the sensor
        uint32_t    DigitalGain;

        /// \brief    Analog Gain of the sensor
        uint32_t    AnalogGain;

        /// \brief    Default constructor which reset all exposure parameters
        CExposureParams() :
            ExposureTime(0),
            DigitalGain(0),
            AnalogGain(0)
            {
            }
    };

    ///////////////////////////////////////////////////////////////////////
    /// \brief    ROI (region of interest) for automatic sensor control  algorithm
    ///////////////////////////////////////////////////////////////////////
    struct CSensorControlROIParams
    {
        /// \brief    If true then ROI is applied in automatic sensor control
        bool UseROI;

        /// \brief    Top left corner of ROI
        CPoint<2, unsigned int> ROITopLeft;

        /// \brief    Bottom right corner of ROI
        CPoint<2, unsigned int> ROIBottomRight;

        /// \brief    Default constructor, when ROITopLeft=ROIBottomRight=0 then ROI is not apply
        CSensorControlROIParams() : UseROI(false), ROITopLeft(0, 0), ROIBottomRight(0, 0) {}
    };

    ///////////////////////////////////////////////////////////////////////
    /// \brief    Sensor control parameters
    ///////////////////////////////////////////////////////////////////////
    struct CSensorControlParams :public CExposureParams
    {

        /// \brief    If this flag is true then automatic algorithm for gain and exposure time adjustment (AGC) is activated.
        bool AutoControl;

        /// \brief     ROI of each sensor
        CSensorControlROIParams Params;

        /// \brief    Default constructor, automatic sensor control is on
        CSensorControlParams() : AutoControl(false){ }
    };

    ///////////////////////////////////////////////////////////////////////
    /// \brief    Auto Exposure parameters
    ///////////////////////////////////////////////////////////////////////
    struct CAutoExposureParams
    {
		// If number (in %) of saturated pixel is above this value, then the frame is saturated
        uint32_t NSatMax;
		// Pixels (only inside ROI) above/equal this value are defined as saturated
        uint32_t GlSat;
		// max allowed value for median
        uint32_t MdMax;
		// min allowed value for median
        uint32_t MdMin;
		// normal ET step
        uint32_t ExposureStepResolution;
		// In case of saturation: threshold (in %) to choose aggressive or normal step
		// If (number (in %) of saturated pixel- nSatMax)> deltaSatMax Can choose aggressive step
        uint32_t DeltaSatMax;
		// aggressive ET step
        uint32_t AggressiveStep;
		// waiting frames between each activation
        uint32_t NoActivationPeriod;
        uint32_t ExposureMax;
        uint32_t ExposureMin;
		// Activate debug prints
        uint32_t Debug;
        // TSNR target value
        float SnrTarget;
        // Memory weight factor for current slope
        float SlopeWeight;
		// Auto exposure algorithm version
        uint32_t AlgVersion;
    };

    enum ESensorLensType
    {
        eSensorLensWide = 0,
        eSensorLensNarrow = 1,
        eSensorLensMax70 = 2,
        eSensorLensMax45 = 3,
        eUnknownLensType = 100
    };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief    Defines the cropping rectangle (Width, Height) and Upper left corner position (StartX, StartY).
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct CCropParams
    {
        uint32_t Width;
        uint32_t Height;
        uint32_t StartX;
        uint32_t StartY;
        CCropParams() :Width(0), Height(0), StartX(0), StartY(0) {}
    };

    ///////////////////////////////////////////////////////////////////////
    /// \brief    ROI (region of interest) for histogram
    ///////////////////////////////////////////////////////////////////////
    struct CROIParams
    {
        /// \brief    Top left corner of ROI
        CPoint<2, unsigned int> ROITopLeft;

        /// \brief    Bottom right corner of ROI
        CPoint<2, unsigned int> ROIBottomRight;

        /// \brief    Default constructor, when ROITopLeft=ROIBottomRight=0 then ROI is not apply
        CROIParams() : ROITopLeft(0, 0), ROIBottomRight(0, 0) {}
    };

    ///////////////////////////////////////////////////////////////////////
    /// \brief    Defines the dimension of the resized image
    ///////////////////////////////////////////////////////////////////////
    struct CChannelDimensions
    {
        uint32_t BufferWidth;   // the width of the output image from the channel after scaling in the chip
        uint32_t BufferHeight;  // the height of the output image from the channel after scaling in the chip
        uint32_t FrameStartX;   // the top left X location of the data inside the output image
        uint32_t FrameStartY;   // the top left Y location of the data inside the output image
        uint32_t FrameWidth;     // the width of the data inside the output image
        uint32_t FrameHeight;     // the height of the data inside the output image
    };

    ////////////////////////////////////////////////////////////////////////
    /// \brief    Represents Inuitive's Sensor
    ///
    /// Role: Enables control of the sensor and generates all kinds of InuDev streams
    ///
    /// Responsibilities:
    ///      1. Singleton object
    ///      2. Derives CBaseStream interface
    ///      3. Creates all kinds of InuDev streams
    ///      4. Provides information about InuSensor connection state
    ///      5. Sensor control
    ///      5. Get and Set different features of InuSensor
    ///
    ////////////////////////////////////////////////////////////////////////
    class CInuSensor
    {

    public:

        enum  ESensorState
        {
            eUninitialized=0,
            eInitialized,
            eStarted,
            eSensorDisconnected,       /// Sensor is disconnected
            eServiceNotConnected
        };

        enum  ESensorTemperatureType
        {
            eSensor1 = 0,
            eSensor2
        };

        /// \brief    Prototype of callback function which is used by the Register method.
        ///Ini
        /// This function is invoked any time a frame is ready, or if an error occurs. It provides information about InuSensor connection state
        /// The callback function arguments are: caller stream object, received Sensor state frame and result code.
        typedef std::function<void(std::shared_ptr<CInuSensor>, EConnectionState,  CInuError)> CallbackFunction;

        virtual ~CInuSensor() {}

        /// \brief    Access to the Singleton object
        INUSTREAMS_API static std::shared_ptr<CInuSensor>  Create(const std::string& iTargetServiceId = std::string());

        // Generate all kinds of InuDev streams
        virtual std::shared_ptr<CDepthStream> CreateDepthStream(uint32_t iChannelID = DEFAULT_CHANNEL_ID) = 0;
        virtual std::shared_ptr<CStereoImageStream> CreateStereoImageStream(uint32_t iChannelID = DEFAULT_CHANNEL_ID) = 0;
        virtual std::shared_ptr<CImageStream> CreateImageStream(uint32_t iChannelID = DEFAULT_CHANNEL_ID) = 0;
        virtual std::shared_ptr<CImuStream>   CreateImuStream(uint32_t iChannelID = DEFAULT_CHANNEL_ID) = 0;
        virtual std::shared_ptr<CUserDefinedStream> CreateUserDefinedStream(const std::string& iChannelName = std::string()) = 0;
        virtual std::shared_ptr<CFeaturesTrackingStream>   CreateFeaturesTrackingStream(const std::string& iStreamerName = std::string()) = 0;
        virtual std::shared_ptr<CSlamStream>  CreateSlamStream(uint32_t iChannelID = DEFAULT_CHANNEL_ID) = 0;
        virtual std::shared_ptr<CCnnAppStream>  CreateCnnAppStream(uint32_t iInputChannelID = DEFAULT_CHANNEL_ID, const std::string& iCnnStreamerOut = std::string()) = 0;
        virtual std::shared_ptr<CCnnAppStream>  CreateCnnAppStream(const std::string& iCnnStreamerOut) = 0;
        virtual std::shared_ptr<CFaceRecognitionStream>  CreateFaceRecognitionStream(uint32_t iChannelID = DEFAULT_CHANNEL_ID) = 0;
        virtual std::shared_ptr<CCnnStream>  CreateCnnStream(const std::string& iStreamerName = std::string()) = 0;
        virtual std::shared_ptr<CPointCloudStream> CreatePointCloudStream(uint32_t iChannelID = DEFAULT_CHANNEL_ID) = 0;
		virtual std::shared_ptr<CHistogramStream> CreateHistogramStream(uint32_t iSensorID) = 0;
        virtual std::shared_ptr<CVoxelsStream> CreateVoxelsStream(uint32_t iChannelID = DEFAULT_CHANNEL_ID) = 0;
        /// \brief    Service initialization.
        ///
        /// Invoked once before initialization of any other InuDev stream.
        /// After invoking Init method the Device is still in low power consumption.
        /// This function will initialize the Device with its default configurations.
        /// \return CInuError    Operation status which indicates on success or failure.
        virtual CInuError Init() = 0;

        /// \brief    Service initialization.
        ///
        /// Invoked once before initialization of any other InuDev stream.
        /// After invoking Init method the Device is still in low power consumption.
        /// This function will provide a map with all available HW channels on which a client application can receive streams of frames
        /// \param[out] oHWInfo    Developer should call the function with a CHwInformation class and will recieved the Device HW configuration.
        /// \param[in] iParams    Initialized the Device with these input parameters. It will be set to all assembled cameras.
        /// \param[in] iCnnParams  CNN network. The service will load the specified CNN network in parallel while InuSensor is Initialized & Startarted to reduce CNN loading time.
	    /// \return CInuError    Operation status which indicates on success or failure.
        virtual CInuError Init(CHwInformation& oHWInfo, const CDeviceParams& iParams = CDeviceParams(), const CCnnLoadParams& iCnnParams = CCnnLoadParams()) = 0;

        /// \brief    Start acquisition of frames.
        ///
        /// Shall be invoked only after the service is successfully initialized and before any request
        /// \param[in out] iParams		- Initialized the Device with these ChannelsParams per channel and return real params with which the server works.
        /// \param[out] oChannelsSize   - Image size that is provided by the Device [Width,Height] according to channelID.
        /// \return CInuError    Operation status.
        virtual CInuError Start(std::map<uint32_t, CChannelSize>& oChannelsSize, std::map<uint32_t, CChannelControlParams>& ioParams) = 0;
        virtual CInuError Start() = 0;

        /// \brief    Service termination.
        ///
        /// Shall be invoked when the service is no longer in use and after frames acquisition has stopped.
        /// \return CInuError    Operation status which indicates success or failure.
        virtual CInuError Terminate() = 0;

        /// \brief    Stop acquisition of frames.
        ///
        /// Shall be invoked after requests for frames are no longer sent and before service termination
        /// (only if Start() was invoked).
        /// \return CInuError    Operation status which indicates success or failure.
        virtual CInuError Stop() = 0;

        /// \brief    Try to connect to Inuitive Sensor.
        ///
        /// Communicate with InuService and try to connect to Inuitive Sensor.
        /// \return CInuError    Error description when connection fails.
        virtual CInuError Connect() = 0;

        /// \brief    Try to disconnect from Inuitive Sensor.
        ///
        /// Stop Communicate to InuService.
        /// \return CInuError    Error description when disconnection fails.
        virtual CInuError Disconnect() = 0;

        /// \brief    Get the connection state of the sensor.
        /// \return EConnectionState
        virtual EConnectionState GetConnectionState() const = 0;

        /// \brief    Get the Sensor state.
        /// \return ESensorState
        virtual ESensorState GetState() const = 0;

        /// \brief    Get the Sensor Temperature.
        /// \param[in] iType  Temperature sensor type.
        /// \param[out] oTemperature    returns the temperature in Celsius .
        /// \return CInuError    Error description of temperature reading.
        virtual CInuError GetSensorTemperature(ESensorTemperatureType iType, float& oTemperature) = 0;

        /// \brief    Registration for receiving InuSensor state notifications (push).
        ///
        /// The provided callback function is called only when the Device state is changed.
        /// \param[in] iCallback    Callback function which is invoked whenever the sensor state is changed.
        /// \return CInuError    Error code, InDev::eOK if operation successfully completed.
        virtual CInuError Register(CallbackFunction iCallback) = 0;

        /// \brief		SW reset of InuSensor, it resets both SW and HW.
        /// \return CInuError If failed to reset the sensor
        virtual CInuError Reset() = 0;

        /// \brief    Get information about the SW and HW components.
        /// \param[out] oVersion    Version description of each component.
        /// \return CInuError    Error code, InDev::eOK if operation successfully completed.
        virtual CInuError GetVersion(std::map<CEntityVersion::EEntitiesID, CEntityVersion>& oVersion) = 0;

        /// \brief    Get Sensor Control (AGC) data.
        ///
        /// It should be called only if after any related stream (e.g. StereoImage, Webcam, etc.) was initiated.
        /// An empty map should be provided, in case the map is not empty the service will clear the map.
        /// \param[out] oSensorsConrolParams, will return with the sensor's control params per sensor.
		/// \param[in]  iProjectorType,  specify the requested projector type by default - ePatterns.
        /// \return CInuError    Error code, InDev::eOK if operation successfully completed.
        virtual CInuError GetSensorControlParams(std::map< uint16_t, CSensorControlParams > & oSensorsConrolParams) const = 0;

        /// \brief    Get Sensor Control (AGC) data.
        ///
        /// It should be called only if after any related stream (e.g. StereoImage, Webcam, etc.) was initiated.
        /// Will return provide CSensorControlParams for specific sensor.
        /// \param[out] oSensorsConrolParams    , will return with the sensor control params for the requested sensor.
        /// \param[in]  iSensorId , specify the requested sensor - in case of stereo - using the first sensor is sufficient.
		/// \param[in]  iProjectorType,  specify the requested projector type by default - ePatterns.
        /// \return CInuError    Error code, InDev::eOK if operation successfully completed.
        virtual CInuError GetSensorControlParams(CSensorControlParams& oSensorsConrolParams, uint16_t iSensorId) const = 0;

        /// \brief    Set Sensor Control (AGC) data.
        ///
        /// It should be called only if after any related stream (e.g. StereoImage, Webcam, etc.) was initiated.
        /// Sets Device control params for specific sensor.
        /// \param[in] iSensorsConrolParams    New Sensor Control parameters.
        /// \param[in] iSensorId    The sensor id.
		/// \param[in] iProjectorType,  specify the requested projector type by default - ePatterns.
        /// \return CInuError    Error code, InDev::eOK if operation successfully completed.
        virtual CInuError SetSensorControlParams(const CSensorControlParams& iSensorsConrolParams, uint16_t iSensorId) const = 0;

        /// \brief    Get Sensor Control Auto Exposure data.
        ///
        /// It should be called only if after any related stream (e.g. StereoImage, Webcam, etc.) was initiated.
        /// Gets Auto Exposure params for specific sensor.
        /// \param[out] oSensorsConrolParams     Auto Exposure parameters.
        /// \param[in] iSensorId    The sensor id.
		/// \param[in] iProjectorType,  specify the requested projector type by default - ePatterns.
        /// \return CInuError    Error code, InDev::eOK if operation successfully completed.
        virtual CInuError GetAutoExposureParams(CAutoExposureParams& oAutoExposureParams, uint16_t iSensorId, EProjectors iProjectorType = ePatterns) const = 0;

        /// \brief    Set Sensor Control Auto Exposure configuration.
        ///
        /// It should be called only if after any related stream (e.g. StereoImage, Webcam, etc.) was initiated.
        /// Sets Auto Exposure params for specific sensor.
        /// \param[in] iAutoExposureParams    New Auto Exposure parameters.
        /// \param[in] iSensorId    The sensor id.
		/// \param[in] iProjectorType,  specify the requested projector type by default - ePatterns.
		/// \return CInuError    Error code, InDev::eOK if operation successfully completed.
        virtual CInuError SetAutoExposureParams(const CAutoExposureParams& iAutoExposureParams, uint16_t iSensorId, EProjectors iProjectorType = ePatterns) = 0;

        /// \brief    Load Registers from input file.
        ///
        /// It should be called only if after the Device was initiated.
        /// The file format should be provided by Inuitive technical staff.
        /// \param[in] iFileName    Input file name provided by Inuitive.
        /// \return CInuError    Error code, InDev::eOK if operation successfully completed.
        virtual CInuError LoadRegistersConfigurationFile(std::string iFileName) const = 0;

        /// \brief		Get Calibration data information
        /// \param[out]  oOpticalData   Output optical data information
        /// \param[int  iCameraName   Optical data of which camera
        /// \param[int  iTemperature  Optical data of which temperature, in case of default the optical data of active calibration is returned
        /// \return CInuError   If InuSensor isn't initialized then eStateError is returned
        virtual CInuError GetCalibrationData(
            CCalibrationData& oOpticalData,
            uint32_t iChannelID = DEFAULT_CHANNEL_ID,
            int iTemperature = std::numeric_limits<int>::max()) const  = 0;

        /// \brief		Set one of the assembled projectors' state
        ///
        /// \param[in]  iLevel : High - high power, Low low power, Off - projector off
        /// \param[in]  EProjectors - Projector name, eNumOfProjectors is illegal value
        /// \return CInuError
        virtual CInuError SetProjectorLevel(EProjectorLevel iLevel, EProjectors iProjectorID) const = 0;

        /// \brief		Get one of the assembled projectors' state
        ///
        /// \param[out]  iLevel : High - high power, Low low power, Off - projector off
        /// \param[out]  EProjectors - Projector name, eNumOfProjectors is illegal value
        /// \return CInuError
        virtual CInuError GetProjectorLevel(EProjectorLevel& iLevel, EProjectors iProjectorID) const = 0;

        /// \brief Record Device streams
        /// \param[in]  iDestinationDirectory Destination directory for recording output. Send empty string to stop recording.
        /// \param[in]  iTemplateName string which will be concatenate to output file name.
        /// \param[in]  iDuration recording time in ms.
        /// \return CInuError eOK indicates that request was successfully processed but it doesn't indicate that recording is completed
        virtual CInuError  Record(const std::string& iDestinationDirectory,
            const std::string& iTemplateName = std::string(),
            uint64_t iDuration = CBaseOutputStream::RECORD_INFINITE) const = 0;

        /// \brief Record Device streams
        /// \param[in]  iDestinationDirectory Destination directory for recording output. Send empty string to stop recording.
        /// \param[in]  iTemplateName string which will be concatenate to output file name.
        /// \return CInuError eOK indicates that request was successfully processed but it doesn't indicate that recording is completed
        virtual CInuError  Snapshot(const std::string& iDestinationDirectory,
            const std::string& iTemplateName = std::string(),
            uint64_t iFileNameIndex = CBaseOutputStream::RECORD_INFINITE) const = 0;

        /// \brief    Load network from input file name.
        /// \param[in] iLoadParams    Loaded network parameters.
        /// \return InuDev::CInuError    Error code, InDev::eOK if operation successfully completed.
        virtual CInuError LoadCnnNetworks(const CCnnLoadParams& iLoadParams) = 0;

        /// \brief   Release all previously loaded CNN networks.
        /// \return InuDev::CInuError    Error code, InDev::eOK if operation successfully completed.
        virtual CInuError ReleaseCnnNetworks(void) = 0;

        /// \brief    GetDeviceTime
        ///
        /// Should be called only after the Device had initiated.
        /// \param[out] oTime  The value generally represents the number of seconds since 00:00 hours, Jan 1, 1970 UTC (i.e., the current unix timestamp).
        /// \return CInuError    Error code, InDev::eOK if operation successfully completed.
        virtual CInuError GetDeviceTime(uint64_t& oTime) const = 0;

        /// \brief    SetChannelCropping
        ///
        /// Set channel as "croppable" & define the cropping rectangle size and position.
        /// The position of the rectangle can be moved in runtime by calling the stream's API SetCroppingROI
        /// \param[in] iChannelId
        /// \param[in] iCropParams defines the size of the cropped rectangle and its position
        /// \return CInuError    Error code, InDev::eOK if operation successfully completed.
        virtual CInuError SetChannelCropping(uint32_t iChannelId, const CCropParams& iCropParams) const = 0;

        /// \brief    SetChannelDimensions
        ///
        /// Enables to change channel dimensions (not all channels supports this operation).
        /// Should be called only after the sensor had started.
        /// \param[in] iChannelID The id of the output channel on which the scaling operation will affect.
        /// \param[in] iChannelDimensions The dimension of the actual data inside the output image after scaling.
        /// \return CInuError    Error code, InDev::eOK if operation successfully completed.
        virtual CInuError SetChannelDimensions(uint32_t iChannelID, const CChannelDimensions& iChannelDimensions) const = 0;

        /// \brief    Set Sensor Histograms ROI (AGC) data.
        ///
        /// It should be called only after the Device was started.
        /// Sets the sensor's histogram's ROIs.
        /// Each sensor has 3 histograms the first histogram in the vector will be used for Automatic Sensor Control
        /// At least one ROI should be provided
        /// \param[in] vector of histograms
        /// \param[in] iSensorId    The sensor id.
        /// \return CInuError    Error code, InDev::eOK if operation successfully completed.
        virtual CInuError SetSensorHistogramsROI(const std::vector<CROIParams>& iHistogramsROI, uint16_t iSensorId) const = 0;

		/// \brief    SetAlternateProjectorMode
		///
		/// Specific API for projector toggle
		/// Should be called after the sensor had started, and before the IR/DEPTH channel is started after the sensor had started.
		/// \param[in] iAlternateModeEnable	AlternateMode Enable/Disable
		/// \param[in] iNumFramesPattern	Number of frames per Pattern mode
		/// \param[in] iNumFramesFlood		Number of frames per Flood mode
		/// \return CInuError    Error code, InDev::eOK if operation successfully completed.
		virtual CInuError SetAlternateProjectorMode(bool iAlternateModeEnable, uint32_t	iNumFramesPattern,  uint32_t	iNumFramesFlood) const = 0;

		/// \brief    GetAlternateProjectorMode
		///
		/// Get specific API for projector toggle
		/// \param[out] oAlternateModeEnable	AlternateMode Enable/Disable
		/// \param[out] oNumFramesPattern		Number of frames per Pattern mode
		/// \param[out] oNumFramesFlood			Number of frames per Flood mode
		/// \return CInuError    Error code, InDev::eOK if operation successfully completed.
		virtual CInuError GetAlternateProjectorMode(bool& oAlternateModeEnable, uint32_t&	oNumFramesPattern, 	uint32_t&	oNumFramesFlood) const = 0;

    public:

        CInuSensor() {}

    };
}

#endif  // __INUSENSOR_H__
