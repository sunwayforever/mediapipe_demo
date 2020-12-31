//
// File : IregFile.h
//

#ifndef __IREGFILE_H__
#define __IREGFILE_H__

#include "CommonUtilitiesDefs.h"
#include "RegistersSet.h"

#include <string>
#include <vector>
#include <map>
#include <limits>

namespace InuCommon
{
    ////////////////////////////////////////////////////////////////////////
    /// \brief    Interface for ireg files 
    ///
    /// Role: Utility class which provides interface for reading and writing
    ///       Inuitive's ireg files format.
    ///
    /// Responsibilities: 
    ///      1. Pack and save Inuitive's ireg file
    ///      2. Read data from Inuitive's ireg file
    ////////////////////////////////////////////////////////////////////////
    class COMMONUTILITIES_API  CIregFile
    {
    public:
        /// \brief    Returned error codes
        enum EError
        {
            eOK = 0,
            eFileOpen,          /// Can't open input ireg file 
            eWriteData,         /// Error in writing ireg buffer 
            eReadData,          /// Error in reading ireg file
            eUnexpectedFormat,  /// Input file format is different than ireg
            eBadRegister        /// Unknown register appeared on ireg file
        };

        /// \brief    Write registers from file (config.ireg). 
        /// \param[out] iBuffer     Collection of registers to write.
        /// \param[in] iFileNamec   Registers file name (config.ireg).
        /// \param[in] iPhase       Registers phase int this input ireg file (A/B)
        /// \return EError    eOK in case of success.
        static EError Write(const std::vector<CRegistersSet::CRegisterData>& iBuffer,
            const std::string& iFileName,
            std::pair<unsigned int, unsigned int>& videoMapSize,
            std::pair<unsigned int, unsigned int>& depthMapSize,
            unsigned int iImageFormat,
            unsigned int iShiftY,
            unsigned int iPhase = std::numeric_limits<unsigned int>::max());

        /// \brief    Load registers from file (config.ireg). 
        /// \param[out] oResult     Collection of loaded registers.
        /// \param[in] iFileNamec   Registers file name (config.ireg).
        /// \param[in] iPhase       Registers phase int this input ireg file (A/B)
        /// \param[in] iIsOdpReg    If this value is true then registers are read also from odp_reg.ireg
        /// \return EError    eOK in case of success.
        static EError Read(std::vector<CRegistersSet::CRegisterData>& oResult,
            const std::string& iFileName,
            std::pair<unsigned int, unsigned int>& videoMapSize,
            std::pair<unsigned int, unsigned int>& depthMapSize,
            std::pair<unsigned int, unsigned int>& webcamSize,
            unsigned int& oImageFormat,
            unsigned int& oShiftY,
            std::string& oConfigIregVer,
            bool iIsOdpReg,
            unsigned int iPhase = std::numeric_limits<unsigned int>::max());

    private:

#ifdef _MSC_VER
#pragma warning (suppress : 4251)
        static std::map<CRegistersSet::RegisterType, std::string> mRegisterTypeToDescription;
#else
        static std::map<CRegistersSet::RegisterType, std::string> mRegisterTypeToDescription;
#endif
    };
}

#endif
