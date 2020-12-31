#ifndef __BASELOGGER_H__
#define __BASELOGGER_H__


#include <string>

namespace InuCommon
{
    ////////////////////////////////////////////////////////////////////////
    /// \brief    Base class (Interface) for all kinds of Loggers   
    /// 
    /// Role: Abstract base class for logging debug information
    ///
    ////////////////////////////////////////////////////////////////////////
    class IBaseLogger
    {
    public:
    	virtual ~IBaseLogger() {}

        /// \brief    Implementation of writing single message to log
        virtual void  Write(const std::string& logMessage) = 0;

        /// \brief    Flush all written message to physical log media
        virtual void  Flush() = 0;

    };
}


#endif
