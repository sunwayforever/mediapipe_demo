//
// File : LutAdapterWrapperB0.h
//

#ifndef __LUTADAPTERWRAPPERB0_H__
#define __LUTADAPTERWRAPPERB0_H__

#include "CommonUtilitiesDefs.h"
#include <string>

using namespace std;

namespace InuCommon
{
    ////////////////////////////////////////////////////////////////////////
    /// \brief    Interface for LutAdapter B0
    ///
    /// Role: Utility class which provides interface for converting DSR & 
    ///       intensity balance files for B0
    ///       
    ///
    /// Responsibilities: 
    ///      1. Read DSR & IB data and convert it.
    ////////////////////////////////////////////////////////////////////////
    class COMMONUTILITIES_API CLutAdapterWrapperB0
    {
    public:

        /// \brief Constructor
        CLutAdapterWrapperB0(void);

        /// \brief Destructor
        ~CLutAdapterWrapperB0(void);

        /// \brief This function handle file convert and interaction with LutAdapter_b0
        static int RunLutConverter(std::string iFilename, std::string iBlockName, int width, int height);

    private:

        static const char* bin_extension;

        static const char* hex_extension;
    };
}

#endif