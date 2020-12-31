
#ifndef __LOGGER_H__
#define __LOGGER_H__


#if ((_MSC_VER < 1700)) //Visual studio version before 2012
#include <boost/thread/thread.hpp>
typedef boost::thread ThreadType;
#else
#include <thread>
typedef std::thread ThreadType;
#endif

#include "OSUtilities.h"
#include "Locks.h"
#include "CommonUtilitiesDefs.h"

#include <list>
#include <memory>
#include <string>

#ifdef _MSC_VER
    #include "Windows.h"
#endif

#ifdef _MSC_VER
#pragma warning(disable : 4251)
#endif

namespace InuCommon 
{
    class IBaseLogger;

    ////////////////////////////////////////////////////////////////////////
    /// \brief    Utility class for saving log information   
    /// 
    /// Role: Abstract base class for logging debug information
    ///
    /// Responsibilities: 
    ///      1. Singleton
    ///      2. Supports various of log outputs
    ///      3. Knows how to write data into log output with low performance overhead
    ///      4. Has a debug level and knows to write message according to its severity
    ///
    ////////////////////////////////////////////////////////////////////////
    class COMMONUTILITIES_API CLogger
    {

    public:

        /// \brief     Indicates destructor was called.
        static bool sDestructorInvoked;

        /// \brief    Severity level
        enum ESeverity 
        {
            eDebug = 1, 
            eInfo, 
            eWarning, 
            eError, 
            eNone
        };

        /// \brief    Log output type
        enum ELogType 
        {
            eAuto = 0,
            eConsole = 1, 
            eFile = 2,
            eDebugView = 4,
        };

        /// \brief    Write log information (by the Singleton object).
        /// \param[in] iLog    Log message.
        /// \param[in] iSevirity    Message sevirity (will be logged only the sevirity is higher than threshold).
        /// \param[in] iPrefix    A prefix string that will be added to log.
        static void  Write(const std::string& iLog,      
                           ESeverity iSevirity,
                           const std::string& iPrefix/*=std::string()*/);

        /// \brief      Set the Logged message threshold for messages with prefix "iMessagePrefix" (only messages with severity > sSeverity will be logged) 
        static void  SetSeverity(const std::string& iMessagePrefix, ESeverity iLevel);

        /// \brief      Get the Logged message threshold. If "iMessagePrefix" is empty then the defualt is returned;
        static ESeverity GetSeverity(const std::string& iMessagePrefix=std::string());

        /// \brief      Check if input severity is above severity threshold  
        static bool  CheckSeverity(ESeverity iSeverity, const std::string& iMessagePrefix=std::string())  { return iSeverity >= GetSeverity(iMessagePrefix); }

        /// \brief      Set and get the log output type. It recreates the Singleton object and the output repository. 
        static void SetLogType(int iLogType = eAuto);
        static int GetLogType()  { return sLogType; }
        
        /// \brief      Set text file logger output path. It recreates the Singleton object and the output repository. 
        static void SetFileLoggerPath(const std::string& iTextFileFolder);

        static void DestroyInstance();

        /// \brief      Write all waiting messages. 
        static void Flush();

        virtual ~CLogger();

    protected: 

        /// \brief    The constructor is protected to maintain the Singleton pattern
        CLogger();

        /// \brief    Stop logging thread
        /// \param[in] iWriteLog   If true then flush all left over messages in queue .
        void  Stop(bool iWriteLog);

    private:

        static const std::string SEVERITY_DEFAULT_PREFIX;

        /// \brief     Singleton instance
        static std::shared_ptr<CLogger>	sTheInstance;

        /// \brief     Synchronization object for singleton creation 
        static BLock sConstructionLocker;

        /// \brief     Threshold for logged messages: only messages with severity > mDebugLevel will be logged.
        ///            It is possible to set severity for each message prefix
        static std::map<std::string,ESeverity> sSeverity;

        /// \brief     Output log type
        static int sLogType;

        /// \brief     All different type of loggers
        std::map<ELogType, IBaseLogger*>  mLoggers;

        /// \brief     All written messages are held in queue and are flushed to output in a different thread.
        std::list<std::string> mMessagesQueue; //log message queue

        /// \brief     Thread that flushes all written messages (in that way written to log has small foot print in run time)
        pthread_t mLoggingThread;

        /// \brief     Synchronization object for Messages Queue management 
        std::shared_ptr<void> mLogMessageEvent;


        /// \brief     Synchronization object for signaling thread finish
        std::shared_ptr<void> mThreadFinishedEvent;

        /// \brief     True it the thread is running  
        bool mLoggingThreadIsRunning;

        /// \brief     Synchronization object for Messages Queue management 
        BLock mQueueLock;

        /// \brief     Access to singleton instance
        static std::shared_ptr<CLogger>   GetTheInstance();

        /// \brief     Create the singleton object  (it is in separate method, in order to achieved more efficient run time) 
        static void   CreateTheInstance(int iLogType);

        /// \brief     Thread function
        static ThreadFuncReturnType STD_FUNC_CALL  	WriteLogsThread(void* iParent);

        /// \brief    Write implementation (it is in separate method, in order to achieved more efficient run time)
        void SingleWrite(const std::string& log, ESeverity sevirity, const std::string& logPrefix);

        /// \brief    Initialize CLoggeer
        bool Init(int iLogType);

        /// \brief     Flushes all waiting messages in queue
        void WriteAllLogs();
    };

    inline std::shared_ptr<CLogger> InuCommon::CLogger::GetTheInstance() 
    {
        //Hold a reference to the static instance.
        if (sTheInstance == nullptr) 
        {
            CreateTheInstance(eAuto);
        }
        return sTheInstance;
    }

    inline void CLogger::Write(const std::string& iLog, ESeverity iSeverity , const std::string& iLogPrefix)
    {
        std::string severityPrefix(SEVERITY_DEFAULT_PREFIX);
        if (sSeverity.find(iLogPrefix) != sSeverity.end())
        {
            severityPrefix = iLogPrefix;
        }
        if (iSeverity >= sSeverity[severityPrefix])
        {
            std::shared_ptr<CLogger> theInstance = GetTheInstance();
            theInstance->SingleWrite(iLog, iSeverity, iLogPrefix);
        }
    }

    inline void  CLogger::SetSeverity(const std::string& iMessagePrefix, ESeverity iLevel)
    {
        sSeverity[iMessagePrefix] = iLevel;
    }

    inline void  CLogger::Flush()
    {
        std::shared_ptr<CLogger> logger = GetTheInstance();
        logger->WriteAllLogs();
    }

    inline  CLogger::ESeverity CLogger::GetSeverity(const std::string& iMessagePrefix)
    {
        if (iMessagePrefix.empty() || (sSeverity.find(iMessagePrefix) == sSeverity.end()))
        {
            if (sSeverity.find(SEVERITY_DEFAULT_PREFIX) == sSeverity.end())
            {
                sSeverity[SEVERITY_DEFAULT_PREFIX] = CLogger::eError;
            }
            return sSeverity[SEVERITY_DEFAULT_PREFIX];
        }
        
        return sSeverity[iMessagePrefix];
    }
}

#ifdef _MSC_VER
#pragma warning(default : 4251)
#endif

#endif
