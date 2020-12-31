
#ifndef __REGISTERSSET_H__
#define __REGISTERSSET_H__

#include "InuSensorDBDefs.h"
#include "TiffFile.h"

#include <string>
#include <vector>
#include <limits>
#include <iostream>

namespace boost { namespace filesystem { class path; } }

#ifdef max
#undef max
#endif

namespace InuCommon
{
    ////////////////////////////////////////////////////////////////////////
    /// \brief    Set of Registers values
    ///
    /// Role: Represents registers set that should be loaded to chip
    ///
    /// Responsibilities: 
    ///      1. Knows how to load/save from file
    ///      2. Access to calibration registers values 
    ////////////////////////////////////////////////////////////////////////
    class INUSENSORSDB_API CRegistersSet 
    {
    public:

        /// \brief  Type of register to be loaded   
        enum RegisterType
        {           
            eRegister = 0,
            eSensor,
            eWait,
            eI2C
        };

        /// \brief   Initial register value
        struct CRegisterData
        {
            // Common data of all different types
            RegisterType mType;
            unsigned int mAddress;
            unsigned int mData;
            unsigned int mPhase;
            unsigned int mMask;

            // Additional data for sensor registers
            unsigned int mSize;
            bool mLeftEye;

            // Additional data for IC2 registers
            unsigned int mI2cNum;
            bool mWrite;
            unsigned int mDeviceAddress;
            unsigned int mAddressSize;  // Reg address size


            CRegisterData() : mType(eRegister), mAddress(0), mData(0), mPhase(1), mMask(std::numeric_limits<unsigned int>::max()), mSize(0), mLeftEye(true), mI2cNum(0), mWrite(0), mDeviceAddress(0), mAddressSize(0) {}

            bool operator==(const CRegisterData& input) const { return (mAddress == input.mAddress && mType == input.mType && mPhase == input.mPhase && mDeviceAddress == input.mDeviceAddress); };
        };

        static CRegistersSet* CreateObject(InuCommon::CTiffFile::EHWType iHwType);

        CRegistersSet* Clone() const;

        virtual ~CRegistersSet() {}

        /// \brief  Load optical data from file system
        virtual EInuSensorsDBErrors Load(const boost::filesystem::path& iCalibrationSetDir, const std::string& iVersion) = 0;

        /// \brief  Load odp reg data(Created by DSR) from file system 
        virtual EInuSensorsDBErrors LoadOdpReg(const boost::filesystem::path& iCalibrationSetDir) = 0;

        /// \brief    brief Get initial registers values.
        /// 
        /// If registers values have not been loaded yet from DB, then it loads it and returns the loaded data 
        /// \return const std::vector<CRegisterData>&    If it fails the returned collection is empty.
        std::vector<CRegisterData>& GetRegistersData() { return mRegistersData; }

        std::vector<CRegisterData>& GetRegistersModifyableData()  { return mRegistersData; }

        /// \brief    brief Get initial registers values as string
        /// \return const std::string    Config.ireg contents 
        std::string GetRegistersDataAsString() const;

        const std::pair<unsigned int, unsigned int>& GetWebcamSize() const { return mWebcamSize; }
        const std::pair<unsigned int, unsigned int>& GetDepthSize() const { return mDepthSize; }
        const std::pair<unsigned int, unsigned int>& GetVideoSize() const { return mVideoSize; }

        void SetVideoSize(const std::pair<unsigned int, unsigned int>& iVideoSize) { mVideoSize = iVideoSize; }
        void SetDepthSize(const std::pair<unsigned int, unsigned int>& iDepthSize) { mDepthSize = iDepthSize; }

        unsigned int GetShiftY() const { return mShiftY;  }

        /// \brief  Save registers data to file system
        EInuSensorsDBErrors Save(const boost::filesystem::path& iCalibrationSetDir);
        
    protected:

#ifdef _MSC_VER
#pragma warning(disable : 4251)
#endif
        std::pair<unsigned int, unsigned int> mWebcamSize;   //<Width,Height>
        std::pair<unsigned int, unsigned int> mDepthSize;    //<channel,<Width,Height>>
        std::pair<unsigned int, unsigned int> mVideoSize; //<channel,<Width,Height>>

        /// \brief  Collection of initial registers values
        std::vector<CRegisterData> mRegistersData;

#ifdef _MSC_VER
#pragma warning(default : 4251)
#endif

        unsigned int mShiftY;

        CRegistersSet() {}

    };

    inline std::ostream& operator<<(std::ostream& oStream, const CRegistersSet::CRegisterData& iRegister)
    {
        oStream << iRegister.mType << " " << std::ios::hex << iRegister.mAddress << " " << iRegister.mData
            << " " << iRegister.mPhase << " " << iRegister.mMask << " " << iRegister.mSize << " " << iRegister.mLeftEye
            << " " << iRegister.mI2cNum << " " << iRegister.mWrite << " " << iRegister.mDeviceAddress
            << " " << iRegister.mAddressSize << std::endl;
        return oStream;
    }

    
    class INUSENSORSDB_API CRegistersSet3000 : public CRegistersSet
    {
    public:
        
        // Implementation of IPersistent abstract methods - can be invoked only be friend CInuDevice class
        virtual EInuSensorsDBErrors Load(const boost::filesystem::path& iCalibrationSetDir, const std::string& iVersion) override;

        virtual EInuSensorsDBErrors LoadOdpReg(const boost::filesystem::path& iCalibrationSetDir) override;

        const std::string& GetConfigIregVersion() const { return mConfigIrgVer; }

    private:

        /// \brief  Config.ireg Version 
#ifdef _MSC_VER
#pragma warning(suppress: 4251)
        std::string mConfigIrgVer;
#else
        std::string mConfigIrgVer;
#endif


        /// \brief    brief Get initial registers values from one ireg file.
        /// \param[in] iIregFileName    Config ireg file name
        /// \param[in] iPhase     Registers phase int this input ireg file (A/B)
        EInuSensorsDBErrors GetRegistersData(const boost::filesystem::path& iIregFileName, bool iIsOdpReg, unsigned int iPhase = std::numeric_limits<unsigned int>::max());
    };

    class INUSENSORSDB_API CRegistersSet4000 : public CRegistersSet
    {
    public:

        // Implementation of IPersistent abstract methods - can be invoked only be friend CInuDevice class
        virtual EInuSensorsDBErrors Load(const boost::filesystem::path& iCalibrationSetDir, const std::string& iVersion) override;

        virtual EInuSensorsDBErrors LoadOdpReg(const boost::filesystem::path& iCalibrationSetDir) override;

    };
}
#endif
