
#ifndef __TEXTFILELOGGER_H__
#define __TEXTFILELOGGER_H__

#include "BaseLogger.h"
#include "OSUtilities.h"
#include <fstream>

namespace InuCommon
{

    ////////////////////////////////////////////////////////////////////////
    /// \brief   Text file logger implementation.
    /// 
    /// Role: Save logged information into file system 
    ///
    /// Responsibilities: 
    ///      1. Implements CLogger interface
    ///      2. Knows how to prevent disk overload 
    ///
    ////////////////////////////////////////////////////////////////////////
    class CTextFileLogger: public IBaseLogger
    {
        // It should be a friend of CLogger because of the private constructor 
        friend class CLogger;
        
    public:

        virtual ~CTextFileLogger();

        /// \brief      Set text file logger output path. It recreates the Singleton object and the output repository. 
        static void SetFileLoggerPath(const std::string& iTextFileFolder);

        COMMONUTILITIES_API static void SetMaxFileSize(unsigned int iNewFileSize)   { sMaxFileSize = iNewFileSize;  }

    private:

        static const unsigned int  MAX_FILE_SIZE = 2 << 20;
        static const unsigned int  MAX_NUMBER_OF_FILES = 10;  // No more than 10 * 2 MB of log file per process.

        /// \brief    Maximum size of each output text file (default is MAX_FILE_SIZE)
        static unsigned int sMaxFileSize;

        /// \brief    Indicates whether the log file folder have been changed
        static bool sIsFileFolderChanged;

        /// \brief    In case the user have defined this path, save the log file in the used defined path, else save in default path        
        static std::string sTextFileFolder;
        
        /// \brief    This counter is required to keep output files in reasonable size. It must be less than MAX_FILE_SIZE.
        unsigned int mCounter; 

        /// \brief    This counter is used when need to generate new file (when mCounter > MAX_FILE_SIZE)
        unsigned int mFileInstance; 


        /// \brief    Output text file        
        std::ofstream mOutputFile;

        CTextFileLogger();

        void OpenFile();

        virtual void  Write(const std::string& logMessage) override;

        virtual void  Flush() override { mOutputFile.flush(); }
    };
}

#endif // __TEXTFILELOGGER_H__

