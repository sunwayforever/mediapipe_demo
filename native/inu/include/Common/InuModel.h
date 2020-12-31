#ifndef __INUMODEL__
#define __INUMODEL__

#include "InuSensorDBDefs.h"

#include "TiffFile.h"

#include <vector>
#include <map>
#include <string>

#ifdef _MSC_VER
#pragma warning(disable:4251)
#endif

//as defined in FW
enum ESluFormat
{
    SLU_PARALLEL_INTERFACE_E  = 0,
    SLU_MIPI_INTERFACE_E      = 1
};

namespace InuCommon
{
    ////////////////////////////////////////////////////////////////////////
    /// \brief    Represents the data associated with Sensor model
    ///
    /// Role: Each Sensor model has pre-defined parameters that are loaded from internal config file
    ///
    /// Responsibilities:
    ///      1. Knows all parameters that are associated with Sensor model
    ///      2. Knows how to load these parameters from internal config file
    ////////////////////////////////////////////////////////////////////////

    class INUSENSORSDB_API CInuModel
    {
    public:

		struct CDpeParams
		{
			std::string fileName;
			uint32_t frameNum;

			CDpeParams(
				std::string iFileName = "",
				uint32_t iFrameNum = 1)
				: fileName(iFileName), frameNum(iFrameNum)
			{}
		};

	public:
        static const std::string CSV_FILE_NAME;

        static const std::string TITLES_NAME;

        static const std::string INUMODELDB_REVISION;

        /// This model name is used when sensor isn't flashed.
        static const std::string EMPTY_MODEL_NAME;

        static std::string  GetInuModelsDBPath();

        CInuModel();

        virtual ~CInuModel(){};

        /// \brief   Initialize the current model object.
        ///
        /// CInuModel attributes are read from internal configuration file
        /// \param[in]     iSensorID    Sensor name that should be loaded
        /// \param[in]     iHwType      HW Revision
        /// \return    false if the object could not be loaded
        EInuSensorsDBErrors Init(const std::string& iModelName, CTiffFile::EHWType iHwType);

        /// \brief   Initialize the current model object.
        ///
        /// CInuModel attributes are read from internal configuration file
        /// \param[in]     iSensorID    Sensor name that should be loaded
        /// \param[in]     iHwType      HW Revision
        /// \return    false if the object could not be loaded
        EInuSensorsDBErrors Init(const std::string& iModelName, const std::string& iHwType);

        /// \brief   Initialize the current model object.
        ///
        /// CInuModel attributes are read from internal configuration file
        /// \param[in]     iModuleData    Sensor data to parse
        /// \return    false if the object could not be parsed
        EInuSensorsDBErrors ParseData(const std::vector<std::string>& iModuleData);

        // -------------------------
        // Access to private members
        // -------------------------

        const std::string &            GetModelName() const { return mModelName; }
        
        const std::string&             GetHwTypeAsString() const { return mHwType; }
        CTiffFile::EHWType             GetHwType() const { return sHwTypeMap[mHwType];};

        const std::string &            GetDepthImprovConfigFullPath() const {return mDIConfigFile;}

        int                            GetExposureTimeMaxValue() const {return mMaxETvalue;}

        CTiffFile::ESensorResolution   GetDefaultResolution(uint32_t iChannelID) const;

        int                            GetDefaultFPS(uint32_t iChannelID) const;

        bool                           GetDefaultMirrorMode() const {return mDefaultMirrorMode;}

        bool                           GetDefaultAutoControl() const {return mDefaultAutoControl;}

        const std::string &            GetSensorCtrlConfigFile() const { return mSensorCtrlConfigFile; }

        bool                           GetStereoSensorControl() const { return mStereoSensorControl; }

        CTiffFile::ETiffFormat	        GetDefaultVideoformat() const {return mDefaultVideoFormat;}

        CTiffFile::ETiffFormat	        GetDefaultWebcamformat() const {return mDefaultWebcamFormat;}

        CTiffFile::ETiffFormat	        GetSensorformat() const {return mSensorFormat;}

        bool                           GetMultiChannelsIsSupported() const { return mMultiChannelsIsSupported; }

        unsigned int                   GetWebCamInterfaceType() const {return mWebcamInterfaceType;}

        unsigned int                   GetStereoInterfaceType() const {return mStereoInterfaceType;}

        CTiffFile::EHTSource	        GetHeadOrientationSource() const {return mHeadOrientationSource;}

        bool                           GetFeaturesTrackingSupport() const { return mFeaturesTracking; }

        bool							GetSensorMasterIsRight() const { return mSensorMasterIsRight; }

        bool							GetSensor0IsRight() const { return mSensor0IsRight; }

        unsigned int                   GetAlternateMode() const { return mAlternateMode; }

        const std::string &           GetConfigIregVersion() const { return mConfigIrgVer; }

		unsigned int                  GetImuType() const { return mImuTypeLocation; }

        const std::pair<int,int>&     GetWebCamFullSize() const { return mWebCamFullSize; }

        const std::pair<int, int>&    GetWebCamVerticalBinningSize() const { return mWebCamVerticalBinningSize; }

        const std::pair<int,int>&     GetWebCamBinningSize() const { return mWebCamBinningSize; }

        const int				       GetOrientationVideo() const { return mOrientationVideo; }

        const int				       GetOrientationWebcam() const { return mOrientationWebcam; }

        const int				       GetNumberOfFishEyeSensors() const { return mNumberOfFishEyeSensors; }

        const std::string &            GetDepthIregFile() const { return mDepthIregFile; }

        const std::string &            GetRGBIregFile() const { return mRGBIregFile; }

		float							GetDefaultAccFPS() const { return mDefaultAccFPS; }

		float							GetDefaultAccScale() const { return mDefaultAccScale; }

		float							GetDefaultGyroFPS() const { return mDefaultGyroFPS; }

		float							GetDefaultGyroScale() const { return mDefaultGyroScale; }

		float							GetDefaultMagnoFPS() const { return mDefaultMagnoFPS; }

		float							GetDefaultMagnoScale() const { return mDefaultMagnoScale; }

        float                             GetDefaultImuTimeAlignment() const { return mDefaultImuTimeAlignment; }

		const std::string &				GetInformationMapFilterConfigFile() const { return mInformationMapFilterConfigFile; }

		const std::string &				GetBlobFilterConfigFile() const { return mBlobFilterConfigFile; }

		const std::string &				GetImageStreamsChannels() const { return mImageStreamsChannels; }

        const std::map<uint32_t, std::string> & GetChannelToStream() const { return mChannelToStream; }

        const std::string		        GetInjectionStreamID() const         { return mInjectionStreamID;       }

        const std::string		        GetFeaturesTrackingStreamID() const  { return mFeaturesTrackingStreamID;       }

        const int                       GetFaceRecognitionOutputChannel() const { return mFaceRecognitionOutputChannel;}

        const int		                GetRGBRescaleChannel() const { return mRGBRescaleChannel; }

        const std::string		        GetCnnAppStreamer() const { return mCnnAppStreamer; }

        const std::string		        GetCnnAppFunction() const { return mCnnAppFunction; }

        const std::string               GetCnnAppInjectionStreamer() const { return mCnnAppInjectionStreamer; }

        const std::string               GetCnnAppInjectionFunc() const { return mCnnAppInjectionFunc; }

		const std::vector<CDpeParams>&	GetDpeParams() const { return mVecDpeParams;	 }

        std::map<std::string, std::string>				GetChannelsInterleaveMode() const { return mChannelsInterleaveMode; }

private:

        /// \brief      Current object unique name (identifier)
        std::string mModelName;

        /// \brief      HW Revision
        std::string mHwType;

        /// \brief      Max Exposure Time value that is allowed to set to this model
        int mMaxETvalue;

        /// \brief      Default resolution that should be used for Depth sensors of this model
        CTiffFile::ESensorResolution mDefaultDepthResolution;

        /// \brief      Default resolution per channel
        std::map<uint32_t, CTiffFile::ESensorResolution> mDefaultResolution;

        /// \brief      Default FPS per channel
        std::map<uint32_t, int> mDefaultFPS;

        /// \brief      Default Video input format (Y Only or YUV packed)
        CTiffFile::ETiffFormat	mDefaultVideoFormat;

        /// \brief      Default Webcam input format (Y Only or RGB565)
        CTiffFile::ETiffFormat	mDefaultWebcamFormat;

        /// \brief      Default Mirror mode for display
        bool mDefaultMirrorMode;

        /// \brief      True if automatic Sensor Control should be used.
        bool mDefaultAutoControl;

        /// \brief     Sensor control config file path.
        std::string mSensorCtrlConfigFile;

        /// \brief     Controlling both stereo sensors is possible.
        bool mStereoSensorControl;

        /// \brief      Depth improvement config full path file.
        std::string mDIConfigFile;

        /// \brief      True if multi channels is supported by current model.
        bool mMultiChannelsIsSupported;

        /// \brief      Sensor input format beofre IAE. The actual sensor format if we turn on IAE bypass
        CTiffFile::ETiffFormat mSensorFormat;

        /// \brief     FW stereo interface type.
        unsigned int mStereoInterfaceType;

        /// \brief     FW Webcam interface type.
        unsigned int mWebcamInterfaceType;

        /// \brief     Head tracking source
        CTiffFile::EHTSource mHeadOrientationSource;

        /// \brief      True if this model allows feature tracking
        bool mFeaturesTracking;

        /// \brief      True if the depth is aligned with right camera
        bool mSensorMasterIsRight;

        /// \brief      True if sensor0 is connected to right camera
        bool mSensor0IsRight;

        /// \brief      AlternateMode None-0 Exposure Based-1 Static Toggling-2
        unsigned int mAlternateMode;

		/// \brief     Information Map Filter ConfigFile path.
		std::string  mInformationMapFilterConfigFile;

		/// \brief     Blob Filter ConfigFile path.
		std::string mBlobFilterConfigFile;

        // For internal use
        static std::map<std::string, CTiffFile::ESensorResolution> sResolutionMap;

        static std::map<std::string, CTiffFile::ETiffFormat> sDefaultVideoFormatMap;

        static std::map<std::string, CTiffFile::EHWType> sHwTypeMap;

        std::map<std::string, CInuModel> mInuModels;

        /// \brief      The video orientation ( 0 - keep original state, 1 - flip horizontal, 2 - flip vertical, 3 - flip horizontal and vertical )
        int mOrientationVideo;

        /// \brief      The webcam orientation ( 0 - keep original state, 1 - flip horizontal, 2 - flip vertical, 3 - flip horizontal and vertical )
        int	mOrientationWebcam;

        /// \brief      Number of fisheye sensors per model
        int	mNumberOfFishEyeSensors;

        /// \brief      Default RGB output channel for face recognition
        int mFaceRecognitionOutputChannel;

        /// \brief      Default RGB chan for Cnn App stream
        int mRGBRescaleChannel;

        /// \brief  determines type of IMU: BMX055, BMI160 or others
        unsigned int mImuTypeLocation;

        /// \brief  Config.ireg Version
        std::string mConfigIrgVer;

        std::pair<int,int> mWebCamFullSize;

        std::pair<int, int> mWebCamVerticalBinningSize;

        std::pair<int,int> mWebCamBinningSize;

        /// \brief     Registers file that is loaded when Video/Depth is started.
        std::string mDepthIregFile;

        /// \brief     Registers file that is loaded when RGB is started.
        std::string mRGBIregFile;

		/// \brief      Default FPS that should be used for IMU: Accelerometer
		float mDefaultAccFPS;

		/// \brief      Default scale that should be used for IMU: Accelerometer
		float mDefaultAccScale;

		/// \brief      Default FPS that should be used for IMU: Gyro
		float mDefaultGyroFPS;

		/// \brief      Default scale that should be used for IMU: Gyro
		float mDefaultGyroScale;

		/// \brief      Default FPS that should be used for IMU: Magnometer
		float mDefaultMagnoFPS;

		/// \brief      Default scale that should be used for IMU: Magnometer
		float mDefaultMagnoScale;

        /// \brief      Default scale that should be used for IMU: Magnometer
        float mDefaultImuTimeAlignment;

		/// \brief      List of all available Image streams and their names
		std::string		mImageStreamsChannels;

        std::map<uint32_t, std::string> mChannelToStream;

        std::string		mInjectionStreamID;

        /// \brief      Default Streamer out name for Cnn App stream
        std::string     mCnnAppStreamer;

        /// \brief      Default function name for Cnn App stream
        std::string     mCnnAppFunction;

        /// \brief      Default stremaer name for Cnn App Injection
        std::string     mCnnAppInjectionStreamer;

        /// \brief      Default function name for Cnn App Injection
        std::string     mCnnAppInjectionFunc;

        /// \brief      Default Stream ID name for Features Tracking stream
        std::string     mFeaturesTrackingStreamID;

		std::vector<CDpeParams> mVecDpeParams;

        /// \brief      List of all available Image streams and their names
        std::map<std::string, std::string> 		mChannelsInterleaveMode;

        /// \brief    Load model from one row in file.
        /// \param[in]  iMdoelName    Model name to load.
        /// \param[out] oModuleData   All model data
        /// \return bool    true if success
        EInuSensorsDBErrors  GetModelData(const std::string& iMdoelName, CTiffFile::EHWType iHwType, std::vector<std::string>& oModuleData);
    };

}
#endif
