/*
* File - HWInformation.h
*
* This file is part of the Inuitive SDK
*
* Copyright (C) 2014 All rights reserved to Inuitive
*
*/

#ifndef __HWINFORMATIONH__
#define __HWINFORMATIONH__

#include "GeneralFrame.h"

#include <string>
#include <map>

namespace InuDev
{
    static const uint32_t DEFAULT_CHANNEL_ID = std::numeric_limits<uint32_t>::max();

    ///////////////////////////////////////////////////////////////////////
    /// \brief    Sensor Models
    ///////////////////////////////////////////////////////////////////////
    enum ESensorModel
    {
        eSensorModel_None,                  //INU_DEFSG_SENSOR_MODEL_NONE_E = 0,
        eSensorModel_AR_130_E = 130,        //INU_DEFSG_SENSOR_MODEL_AR_130_E = 130,
        eSensorModel_AR_134_E = 134,        //INU_DEFSG_SENSOR_MODEL_AR_134_E = 134,
        eSensorModel_AR_135_E = 135,        //INU_DEFSG_SENSOR_MODEL_AR_135_E = 135,
        eSensorModel_AR_135X_E = 136,       //INU_DEFSG_SENSOR_MODEL_AR_135X_E = 136,
        eSensorModel_APTINA_1040_E = 1040,  //INU_DEFSG_SENSOR_MODEL_APTINA_1040_E = 1040,
        eSensorModel_OV_7251_E = 7251,      //INU_DEFSG_SENSOR_MODEL_OV_7251_E = 7251,
        eSensorModel_OV_2685_E = 2685,      //INU_DEFSG_SENSOR_MODEL_OV_2685_E = 2685,
        eSensorModel_OV_9282_E = 9282,      //INU_DEFSG_SENSOR_MODEL_OV_9282_E = 9282,
        eSensorModel_AUTO_DETECT_E = 9999   //INU_DEFSG_SENSOR_MODEL_AUTO_DETECT_E = 9999
    };

    ///////////////////////////////////////////////////////////////////////
    /// \brief    Sensor Role
    ///////////////////////////////////////////////////////////////////////
    enum ESensorRole
    {
        eSensorRoleLeft,            //INU_DEFSG_ROLE_LEFT_E = 0,
        eSensorRoleRight,           //INU_DEFSG_ROLE_RIGHT_E = 1,
        eSensorRoleMono             //INU_DEFSG_ROLE_COLOR_E = 2
    };

    ///////////////////////////////////////////////////////////////////////
    /// \brief    HW Channels Types
    ///////////////////////////////////////////////////////////////////////
    enum EChannelType
    {
        eUnknownChannelType = 0,
        eRgbChannel = 1,
        eFisheyeChannel = 2,
        eStereoChannel = 3,
        eDepthChannel = 4,
        eFeaturesTrackingChannel = 5,
        eNumChannelTypes = 6,
    };

    ///////////////////////////////////////////////////////////////////////
    /// \brief    All resolutions supported by Inuitive Sensor
    ///////////////////////////////////////////////////////////////////////
    enum ESensorResolution
    {
        eDefaultResolution = 0,		///< Sensor default resolutions
        eBinning = 1,				///< Sensor's binning mode (reduced resolution provided by sensor)
        eVerticalBinning = 2,		///< Vertical binning resolution
        eFull = 3,					///< Full sensor resolution
        eAlternate = 4,				///< Alternating resolutions
    };

    //enum EInterleaveMode
    enum EInterleaveMode
    {
        eNotSupported, // this value will be returned in InuSensor::Init CHwInfo for each channel which doesn't support interleave
        eRightSensor,  // get data only from right sensor
        eLeftSensor,   // get data only from left sensor
        eInterleave,   // get data from both sensors
        eDefaultInterleaveMode // when this value is provided the configuration will be taken from InuModelDB.csv
    };

    ///////////////////////////////////////////////////////////////////////////
    /// \brief    Channel Params
    ///////////////////////////////////////////////////////////////////////////
    struct CChannelControlParams
    {
        static constexpr int USE_DEFAULT_FPS = -1;

        /// \brief    Sensor Resolution
        ESensorResolution SensorRes;

        /// \brief    Frame rate (number of frames per second)
        uint32_t FPS;

        /// \brief    size of Chunk
		uint32_t ChunkSize;

        /// \brief    Sets which sensors will send frames when stereo sensors are attached to channel.
        EInterleaveMode InterleaveMode;

        /// \brief    Default constructor which defines the default FPS and resolution
        CChannelControlParams(ESensorResolution iResolution = eDefaultResolution, uint32_t iFPS = (uint32_t)USE_DEFAULT_FPS, uint32_t iChunkSize = 0)
            : SensorRes(iResolution)
            , FPS(iFPS)
			, ChunkSize(iChunkSize)
            , InterleaveMode(eDefaultInterleaveMode)
        {}
    };

    ///////////////////////////////////////////////////////////////////////////
    /// \brief   Sensor's info
    ///////////////////////////////////////////////////////////////////////////
    struct CSensorParams
    {
        uint32_t Id;
        int32_t LensType;
        ESensorModel Model;
        ESensorRole  Role;
        bool		 SupportHistogram;
        std::vector<uint32_t> ConnectedChannels;

        CSensorParams(uint32_t iSensorId = 0,
            int32_t iLensType = -1,
            ESensorModel iSensorModel = eSensorModel_None,
            ESensorRole iRole = eSensorRoleMono,
            bool iSupportHistogram = true)
            : Id(iSensorId)
            , LensType(iLensType)
            , Model(iSensorModel)
            , Role(iRole)
            , SupportHistogram(iSupportHistogram)
        {}
    };

    ///////////////////////////////////////////////////////////////////////////
    /// \brief   Injector's info
    ///////////////////////////////////////////////////////////////////////////
    struct CInjectorParams
    {
        std::string StreamerName;
        std::vector<uint32_t> ConnectedChannels;

        uint32_t Id;
        CInjectorParams(uint32_t iInjectorId = DEFAULT_CHANNEL_ID)
            : Id(iInjectorId)
        {}
    };

    ///////////////////////////////////////////////////////////////////////////
    /// \brief    HW Channel from which frames are streamed to the application
    ///////////////////////////////////////////////////////////////////////////
    struct CHwChannel
    {

        EChannelType			ChannelType;
        uint32_t				ChannelId;
        CChannelControlParams	ChannelControlParams;
        bool					FeaturesTrackingPresentationChannel;

        /// \brief    a vector of the sensors which feeds the channel, maps between sensor id to the SensorParams
        std::vector<uint32_t> ChannelSensorsID;

        /// \brief    a vector of injectors which feeds the channel, maps between injector id to the CInjectorParams
        std::vector<std::string>  ChannelInjectorsID;

        CHwChannel(EChannelType iChannelType = eUnknownChannelType,
            uint32_t iChannelId = 0,
            bool iFeaturesTrackingPresentationChannel = false)
            : ChannelType(iChannelType)
            , ChannelId(iChannelId)
            , ChannelControlParams()
            , FeaturesTrackingPresentationChannel(iFeaturesTrackingPresentationChannel)
        {}
    };

    class CHwInformation
    {

        friend class CSensorStreamImp;

    private:

        /// \brief Channels Map. Key: Channel ID - Value: CHwChannel
        std::map<uint32_t, CHwChannel> mChannels;

        /// \brief Sensors Map. Key: Sensor ID - Value: SensorParams
        std::map<uint32_t, CSensorParams> mSensors;

        /// \brief Injectors map. Key: Streamer name - Value: Injector params
        std::map<std::string, CInjectorParams> mInjectors;

    public:

        ///////////////////////////////////////////////////////////////////////////
        /// \brief    Service functions
        ///////////////////////////////////////////////////////////////////////////

        /// \brief    Get active channels
        ///
        /// \return   std::map<uint32_t, CHwChannel>& map of active channels.
        INUSTREAMS_API const std::map<uint32_t, CHwChannel>& GetChannels() const { return mChannels; }

        /// \brief    Get active sensors
        ///
        /// \return   std::map<uint32_t, CSensorParams>& map of active sensors.
        INUSTREAMS_API const std::map<uint32_t, CSensorParams>& GetSensors() const { return mSensors; }

        /// \brief    Get active injectors
        ///
        /// \return   std::map<std::string, CInjectorParams>& map of active injectors.
        INUSTREAMS_API const std::map<std::string, CInjectorParams>& GetInjectors() const { return mInjectors; }

        /// \brief    Get Injectors for a given channel
        ///
        /// \param[in] iChannelID    Input channel ID.
        /// \param[out] oInjectors channel's injectors
        /// \return CInuError    Error code, InDev::eOK if operation successfully completed.
        INUSTREAMS_API CInuError GetInjectorsPerChannel(uint32_t iChannelID, std::map<std::string, CInjectorParams>& oInjectors) const;

        /// \brief    Get Injectors for a given channel
        ///
        /// \param[in] iChannelID Input channel ID.
        /// \return channel's injectors.
        INUSTREAMS_API std::map<std::string, CInjectorParams> GetInjectorsPerChannel(uint32_t iChannelID) const;

        /// \brief    Get Sensors for a given channel
        ///
        /// \param[in] iChannelID    Input channel ID.
        /// \param[out] oSensorParams Returns channel's sensors
        /// \return CInuError    Error code, InDev::eOK if operation successfully completed.
        INUSTREAMS_API CInuError GetSensorsPerChannel(uint32_t iChannelID, std::map<uint32_t, CSensorParams>& oSensorParams) const;

        /// \brief    Get Sensors for a given channel
        ///
        /// \param[in] iChannelID    Input channel ID.
        /// \return CInuError    Returns channel's sensors.
        INUSTREAMS_API std::map<uint32_t, CSensorParams> GetSensorsPerChannel(uint32_t iChannelID) const;

        /// \brief    Get channel for a given sensorID
        ///
        /// \param[in] iSensorID    Input sensor ID.
        /// \param[out] oChannels Returns a map of (ChannelID, CHWChannel) Of the channels related to the sensor
        /// \return CInuError    Error code, InDev::eOK if operation successfully completed.
        INUSTREAMS_API CInuError GetChannelsPerSensor(uint32_t iSensorID, std::map<uint32_t, CHwChannel>& oChannels) const;

        /// \brief    Get channel for a given sensorID
        ///
        /// \param[in] iSensorID    Input sensor ID.
        /// \return CInuError    Returns a map of (ChannelID, CHWChannel) Of the channels related to the sensor.
        INUSTREAMS_API std::map<uint32_t, CHwChannel> GetChannelsPerSensor(uint32_t iSensorID) const;
    };
}

#endif // __HWINFORMATIONH__
