//
// File : LutAdapterWrapperA0.h
//

#ifndef __LUTADAPTERWRAPPERA0_H__
#define __LUTADAPTERWRAPPERA0_H__

#include "CommonUtilitiesDefs.h"
#include <string>

using namespace std;

namespace InuCommon
{
    ////////////////////////////////////////////////////////////////////////
    /// \brief    Interface for LutAdapter A0
    ///
    /// Role: Utility class which provides interface for converting DSR & 
    ///       intensity balance files for A0
    ///       
    ///
    /// Responsibilities: 
    ///      1. Read DSR & IB data and convert it.
    ////////////////////////////////////////////////////////////////////////
    class COMMONUTILITIES_API CLutAdapterWrapperA0
    {
    public:

        /// \brief Constructor
        CLutAdapterWrapperA0(void);

        /// \brief Destructor
        ~CLutAdapterWrapperA0(void);

        /// \brief This function handle file convert and interaction with LutAdapter_a0
        static int RunLutConverter(std::string iPath, std::string iBlockName, std::string iResolutionName);

    private:

        static const char* bin_extension;

        static const char* hex_extension;
    };
}

#endif