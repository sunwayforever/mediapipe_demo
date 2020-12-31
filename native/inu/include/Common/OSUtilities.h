//
// File : OSUtilitiess.h
//

#ifndef __OSUITLITIES_H__
#define __OSUITLITIES_H__

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include "CommonUtilitiesDefs.h"

#ifdef _WIN32
#include <windows.h>
#include <Mmsystem.h>
#include <stdint.h>

#if defined(_MSC_VER) && (_MSC_VER < 1700)
#include <boost/chrono.hpp>
#else
#include <chrono>
#endif

#else
#include <pthread.h>
#include <dlfcn.h>
#include <sys/stat.h>
#include <stdarg.h>
#include <cstring>
#include <cstdio>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <limits>
#include <time.h>

#if  !defined(__arm__) && !defined(__aarch64__)
#include "cpuid.h"
#endif
#include <sys/syscall.h>

#ifdef __ANDROID__
#include <boost/chrono.hpp>
#else
#include <chrono>
#endif

#endif

#include <string>
#include <vector>
#include <limits>
#include <stdint.h>

#ifdef max
#undef max
#endif

#ifdef _WIN32
typedef unsigned long ThreadFuncReturnType;
typedef ThreadFuncReturnType (__stdcall THREAD_FUNC)(void * threadParameter);
typedef void* pthread_t;
typedef void* SM_HANDLE;
#define INFINITE_TIMEOUT INFINITE
#define STD_FUNC_CALL __stdcall
#define C_FUNC_CALL __cdecl
#else
typedef void* ThreadFuncReturnType;
typedef ThreadFuncReturnType (THREAD_FUNC)(void * threadParameter);
typedef pthread_t HANDLE;
typedef void* SM_HANDLE;
typedef unsigned long DWORD;
#define INFINITE_TIMEOUT std::numeric_limits<unsigned int>::max()
#ifndef MAX_PATH
#define MAX_PATH 260
#endif
#define STD_FUNC_CALL
#define C_FUNC_CALL
#endif

#ifndef CHAR_BIT
#define CHAR_BIT 8
#endif

#ifdef __clang__
#include <thread>
#else
// forward declaration
namespace std {
    class thread;
}
#endif

namespace InuCommon {

    ///////////////////////////////////////////////////////////////////////
    // Class: COsUtilities
    // Role: Utility class which wrap all OS dependent code
    // Responsibilities:
    // Comments:    Utility class is a class which all its methods are static
    ///////////////////////////////////////////////////////////////////////
    class COMMONUTILITIES_API COsUtilities
    {

    public:
        //implementation for min and max without branching
        inline static int minNoBranch(int x, int y)
        {
            return y + ((x - y) & ((x - y) >>(sizeof(int) * CHAR_BIT - 1)));
        }

        inline static int maxNoBranch(int x, int y)
        {
            return x - ((x - y) & ((x - y) >> (sizeof(int) * CHAR_BIT - 1)));
        }

        // Logical bit select
        template <typename T>
        inline static T bitselect2(T condition, T truereturnvalue, T falsereturnvalue)
        {
            return condition ? truereturnvalue : falsereturnvalue;
        }

        //equivalent to trinary operator ( a >b : c ? d) , no branching
        inline static int bitselect(int condition, int truereturnvalue, int falsereturnvalue)
        {
            return (truereturnvalue & -condition) | (falsereturnvalue & ~(-condition)); //a when TRUE and b when FALSE
        }

        template <typename T, int N>
        inline static void ArrayCopy(T (&destination)[N] , const T  (&source)[N])
        {
            COsUtilities::memcpy(&destination[0],N * sizeof(T), &source[0],N * sizeof(T));
        }

#if defined(__GNUC__) || !(defined(_UNICODE) || defined(UNICODE))
        inline static const std::string& ConvertEncoding(const std::string& iFilename)
        {
            return iFilename;
        }

        inline static const std::string& ConvertEncodingBack(const std::string& iFilename)
        {
            return iFilename;
        }


#else

        //convert UTF8 to UTF16
        inline static std::wstring ConvertEncoding(const std::string& iFilename);

        inline static std::string ConvertEncodingBack(const std::wstring& iFilename);

#endif

       static void             fopen(FILE*& file,  const char* filename, const char* mode);
        //----------------------------
        // Common interface for all OS
        //----------------------------

#ifdef _MSC_VER
#pragma warning(disable : 4251)
#endif

        // '\' for Windows, '/' for MAC/Linux
        static const char   PATH_SEPARATOR;

        static const int DEADLOCK_TIMEOUT;

        static const std::string  INUITIVE_PATH;

#ifdef _MSC_VER
#pragma warning(default : 4251)
#endif

        //
        // Safe standard C library functions
        //
        static void             memcpy(void* dest, size_t destSize, const void* src, size_t count);
        static void             strcpy(char* dest, size_t destSize, const char* src);
        static void             strncpy(char* dest, size_t destSize, const char* src, size_t count);

        static std::string      getenv(const std::string& varname);

        //
        // Execution control functions
        //
        static void             sleep(unsigned long milliSeconds);
        // Sleep for predefined period
        static void             Reschedule()  { sleep(1); }
        static pthread_t        CreateThread(THREAD_FUNC threadFunc, void * params)  ;
        static bool             WaitForThreadTermination(pthread_t& iThread, unsigned int iTimeOut=INFINITE_TIMEOUT);        // true if termination succeeds
        static bool             WaitForThreadTermination(pthread_t& iThread, DWORD& exitCode, unsigned int iTimeOut=INFINITE_TIMEOUT);
        static bool             WaitForThreadTermination(std::thread& iThread);

        static bool             IsThreadAlive(pthread_t& iThread);

        //
        // File System
        //
        static bool             CreateDir(const std::string& name);
        static bool             FileDelete(const std::string& name);
        static bool             FileExists(const std::string& name);
        static bool             FileCopy(const std::string& source, const std::string& dest);
        static void             CleanupFolder(const std::string& folder, const std::string& iContainsString,int iNumOfFilesToLeave=0);

        struct FileIndexPair
        {
            static int TimeStampCompare(const FileIndexPair& a, const FileIndexPair& b) { return a.mIndex < b.mIndex; }
            static int FilenameCompare(const FileIndexPair& a, const FileIndexPair& b) { return a.mFilename < b.mFilename; }

            std::string mFilename;
            uint64_t mIndex;
        };

        typedef std::vector<FileIndexPair> RecordingFileList;

        // If iPrefix is not empty then only files with this prefix are returned
        static bool             ListAllFiles(const std::string& iDirName, const std::string& iExtension, RecordingFileList& oList,
                                             const std::string& iPrefix=std::string(), bool iFullPath = true);

        // If index != -1 and fileName doesn't end with .raw then this index is concatenate to fileName
        static bool             SaveRaw(const unsigned char* Buffer, int iBufferSize, const std::string& fileName, uint64_t index = std::numeric_limits<uint32_t>::max() );

        // Save buffer as bmp file, padded size is the size of Buffer in bytes. If "index" != -1 it will be concatenate to "baseFileName"
        // baseFileName should not ends with .bmp
        static bool             SaveBMP(const unsigned char* Buffer, int width, int height, int bitsPerPixel, long sizeInBytes, const std::string& baseFileName, uint64_t index = std::numeric_limits<uint64_t>::max());
        // Read buffer from bmp file - buffer should be delete by the caller/ If "index" != -1 it will be concatenate to "baseFileName"
        static unsigned char*   LoadBMP (int& width, int& height, long& size, const std::string& baseFileName, uint64_t index = std::numeric_limits<uint64_t>::max());

        /// \brief      Generate file name that is saved by SaveBMP, SaveRaw and Save tiff
        static std::string BuildFileName(const std::string& iInputFileName, const std::string& iExtension, uint64_t iIndex);


        static std::string      GetCommonConfigPath();
        static std::string      GetRoamingUserPath();
        static std::string      GetLocalUserPath();
        static std::string      GetTempFolder();
        static std::string      GetProgramDataFolder();
        //
        // Synchronization objects
        //
        enum ESyncObjectType { eMutex, eCriticalSection };

        static long             InterlockedCompareExchange( volatile long* iTaget, long iValue, long iComparand);
        //
        // Event Functions
        //
        static void*            EventCreate(bool useManualReset = false)  ;
        static bool             EventDestroy(void* hEvent) ;
        static bool             EventSet(void* hEvent) ;                             // signal the event
        static bool             EventReset(void* hEvent);                            // remove signal

        enum EWaitResults
        {
            eEvent,
            eCancel,
            eTimeout
        };

        // wait for the event to get signaled, returns false in case of timeout
        static EWaitResults     EventWait(void* hEvent, unsigned long timeout=INFINITE_TIMEOUT);

        // cancel the event
        static bool      EventCancel(void* hEvent);

        //wait for event and check for thread liveness every iInretval milliseconds
        //Thread termination may occur when closing the process without properly finishing the thread and reaising the event
        static bool      WaitForEventOrThreadTermination(pthread_t& iThread, void* iEvent, unsigned int iTimeOut = INFINITE_TIMEOUT, unsigned int iInretval = 5 /*ms*/);


        //
        // Shared memory
        //
        // create (or open an existing) shared memory object with the specified name
        static SM_HANDLE        SharedMemoryCreate(const std::string& name, unsigned long size, bool& createdHere);
        // release the shared memory object
        static bool             SharedMemoryRelease(SM_HANDLE hSMHandle);
        // obtain the address of the shared memory after mapping it to the process' address space
        static void*            SharedMemoryAddress(SM_HANDLE hSMHandle);
        // release the mapped shared memory
        static bool             SharedMemoryAddressRelease(void* ptr);

        //
        // Atomic number operations
        //
        static long             FetchAndIncrement(long * pValue);       // atomically increments by one
        static long             FetchAndDecrement(long * pValue);       // atomically decrements by one
        static long             CompareAndExchange(volatile long * pValue, long newValue, long comparand);  // atomically compare and exchange values if identical

        //
        // Library
        //
        static void*	        LoadTheLibrary(const std::string& libName);
		static int 		        FreeTheLibrary(void* iLibHandle);
		static void*            GetFunctionEntryPoint(void* iLibHandle, const std::string& iFuncName);
        static std::string      GetLastErrorText();

        //
        // OS
        //
        static uint32_t 	ProcessID();
        static int32_t 	    ThreadID();
        // Returns true if current thread and input thread handle are equal
        static bool 	    ThreadsAreEqual(HANDLE handle);

        static unsigned long 	GetCurrentSessionID(bool&);
        static bool             IsSessionZeroIsolate();

        static void            GetInstallationPath(std::string& installationPath);

        enum EOptimizationLevel { eNone=0, eSSE, eAVX, eNumberOfOptimizationLevels };

        static const int AVX_PADDING = 32;

        // Returns true if SSE2 is supported by current machine and input width (in bytes) is multiplication of 16 (128 bits)
        // iRequestedLevel enables to enforce lower level of optimization
        static bool             SSE4IsSupported(unsigned int iWidth, EOptimizationLevel iRequestedLevel = eAVX);

        // Returns true if AVX is supported by current machine and input width (in bytes) is multiplication of 32 (256 bits)
        // iRequestedLevel enables to enforce lower level of optimization
        static bool             AVXIsSupported(unsigned int iWidth, EOptimizationLevel iRequestedLevel = eAVX);


        // Returns true if process "procName" is running
        static bool				IsProcessRunning(const std::string& procName);
        static bool             IsProcessRunning(uint32_t procID);
        static uint32_t         GetProcessID(const std::string& procName);

        static void* AllocteBuffer(unsigned int iSize, unsigned int iAlignement = 1);
        static void FreeBuffer(void* buffer);
        static void FreeBuffer(void* buffer, unsigned int iSize);

        // Time

        // Time resolution
        enum ETimeUntis { eSeconds, eMilli, eMicro, eNano };

        // "Wall Clock" of the system (global time), might be accurate up to +-20 ms (depends on SO).
        // Returns the duration since system's clock epoch which can be translate to current "physical" time.
        // It may not be monotonic.
        static uint64_t    GetTimeSystemClock(ETimeUntis iUnits = eMilli);

         // Most accurate time which should be used for benchmarks or time interval measurements.
        static uint64_t    GetTimeHighRes(ETimeUntis iUnits = eNano);

        static std::string GetDLLFileName();
        static std::string GetDLLFullPathFileName();
        static std::string GetDLLFullPathFileName(const std::string& dllName);

        static std::string GetExeFileName();
        static std::string GetExeFullPathFileName();
        static std::string GetExeFullPathFileName(const std::string& procName);

        static bool IsDebuggerPresent();

        /// \brief    Which version that is represented by input strings is newer.
        /// \param[in] iVersionStrFirst    First version string to compare.
        /// \param[in] iVersionStrSecond    Second version string to compare.
        /// \return int    (-1) - first is newer (higher number), (0) - equal, (1) - second is newer.
        static int VersionsStringCompare(const std::string& iVersionStrFirst, const std::string& iVersionStrSecond);

#ifdef _WIN32
        /// \brief Get EXE/DLL file's version as string
        /// \param[in] fileName   File to get version of
        /// \return string        Version string
        static std::string COsUtilities::GetVersion(const std::string fileName);
#endif

    private:
#ifdef _WIN32
        static std::string      GetDLLFileNameImp(HINSTANCE instance);
#endif
        static bool             SetObjectToLowIntegrity(HANDLE hObject);
    };

    inline uint64_t InuCommon::COsUtilities::GetTimeSystemClock(ETimeUntis iUnits)
    {
#if defined(__ANDROID__) || (defined(_MSC_VER) && (_MSC_VER < 1700))
        if (iUnits == eMilli)
            return boost::chrono::duration_cast<boost::chrono::milliseconds>(boost::chrono::system_clock::now().time_since_epoch()).count();
        if (iUnits == eNano)
            return boost::chrono::duration_cast<boost::chrono::nanoseconds>(boost::chrono::system_clock::now().time_since_epoch()).count();
        if (iUnits == eMicro)
            return boost::chrono::duration_cast<boost::chrono::microseconds>(boost::chrono::system_clock::now().time_since_epoch()).count();
        return boost::chrono::duration_cast<boost::chrono::seconds>(boost::chrono::system_clock::now().time_since_epoch()).count();
#else
        if (iUnits == eMilli)
            return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        if (iUnits == eNano)
            return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        if (iUnits == eMicro)
            return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        return std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
#endif
   }

    inline uint64_t InuCommon::COsUtilities::GetTimeHighRes(ETimeUntis iUnits)
    {
#if defined(__ANDROID__) || (defined(_MSC_VER) && (_MSC_VER < 1700))
        if (iUnits == eNano)
            return boost::chrono::duration_cast<boost::chrono::nanoseconds>(boost::chrono::high_resolution_clock::now().time_since_epoch()).count();
        if (iUnits == eMicro)
            return boost::chrono::duration_cast<boost::chrono::microseconds>(boost::chrono::high_resolution_clock::now().time_since_epoch()).count();
        if (iUnits == eMilli)
            return boost::chrono::duration_cast<boost::chrono::milliseconds>(boost::chrono::high_resolution_clock::now().time_since_epoch()).count();
        return boost::chrono::duration_cast<boost::chrono::seconds>(boost::chrono::high_resolution_clock::now().time_since_epoch()).count();
#else
        if (iUnits == eNano)
            return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
        if (iUnits == eMicro)
            return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
        if (iUnits == eMilli)
            return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
        return std::chrono::duration_cast<std::chrono::seconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
#endif
    }
}

#include "OSUtilitiesGCC.h"
#include "OSUtilitiesVS.h"

#endif
