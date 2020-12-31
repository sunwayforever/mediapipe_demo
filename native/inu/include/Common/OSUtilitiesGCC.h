//
// File : OSUtilitiess.h
//

#ifndef __OSUITLITIESGCC_H__
#define __OSUITLITIESGCC_H__

#include "OSUtilities.h"

#ifdef __GNUC__

// This definition comes to eleminate many warnings from boost of
// [-Wunused-variable]
#define BOOST_SYSTEM_NO_DEPRECATED 1

#define FOR_EACH(var, range) for (var : range)
#include <pwd.h>

#include <boost/filesystem.hpp>

#if !defined(__arm__) && !defined(__aarch64__)
#include <smmintrin.h>
#endif

#ifdef __ANDROID__
#include "boost/lexical_cast.hpp"

namespace std {
template <typename T>
inline std::string to_string(T input) {
    return boost::lexical_cast<std::string>(input);
}

#ifdef __ANDROID_API__
#if __ANDROID_API__ < 23
inline unsigned long stoul(const std::string& str) {
    std::istringstream stream(str);
    unsigned long val;
    stream >> val;
    return val;
}

inline unsigned long stoul(const std::string& str, std::size_t*,
                           int base = 10) {
    if (base == 16) {
        std::istringstream stream(str);
        unsigned long val;
        stream >> std::hex >> val;
        return val;
    } else if (base == 10) {
        return std::stoul(str);
    }
    return 0;
}

inline unsigned long stof(const std::string& str) {
    std::istringstream stream(str);
    float val;
    stream >> val;
    return val;
}
#endif
#endif
}  // namespace std

inline int wctomb(char* s, wchar_t wc) { return wcrtomb(s, wc, NULL); }
inline int mbtowc(wchar_t* pwc, const char* s, size_t n) {
    return mbrtowc(pwc, s, n, NULL);
}

#endif

namespace InuCommon {

class COsUtilities;

inline uint32_t COsUtilities::ProcessID() { return getpid(); }

inline int32_t COsUtilities::ThreadID() {
#if defined(__ANDROID__)
    return syscall(__NR_gettid);
#else
    return syscall(SYS_gettid);
#endif
}

inline void COsUtilities::memcpy(void* dest, size_t destSize, const void* src,
                                 size_t count) {
    if (destSize < count) throw "Buffer too Small";
    ::memcpy(dest, src, count);
}

inline void COsUtilities::strcpy(char* dest, size_t destSize, const char* src) {
    if (destSize < strlen(src) + 1) throw "Buffer too Small";
    ::strcpy(dest, src);
}

inline void COsUtilities::strncpy(char* dest, size_t destSize, const char* src,
                                  size_t count) {
    if (destSize < count + 1) throw "Buffer too Small";
    ::strncpy(dest, src, count);
}

inline void COsUtilities::fopen(FILE*& file, const char* filename,
                                const char* mode) {
    // There is no safe version of fopen on Linux/MAC
    file = ::fopen(filename, mode);
}

inline std::string COsUtilities::GetProgramDataFolder() {
    return GetTempFolder();
}

inline bool COsUtilities::ThreadsAreEqual(HANDLE handle) {
    // In linux we're keeping tid anyway.
    return pthread_equal(pthread_self(), handle);
}

inline bool COsUtilities::WaitForThreadTermination(
    pthread_t& hThread, unsigned int timeout)  // out in miliseconds
{
    DWORD exitCode;
    return WaitForThreadTermination(hThread, exitCode, timeout);
}

inline bool COsUtilities::IsThreadAlive(pthread_t& iThread) {
#ifdef __ANDROID__
    bool isAlive = (pthread_kill(iThread, 0) != ESRCH);  // checks thread state
    if (!isAlive) {
        // Thread isn't alive, we should clear is memory
        void* status;
        pthread_join(iThread, &status);
    }
    return isAlive;
#else
    void* status;

    // If we wait for more than 0, then the thread will join and the memory will
    // be cleaned
    // !! The timeout must be at least 1 ms, otherwise it doesn't do join on
    // Linux X64 !!
    timespec linuxTimeout;
    clock_gettime(CLOCK_REALTIME, &linuxTimeout);
    linuxTimeout.tv_nsec += 1000000;  //
    int res = pthread_timedjoin_np(iThread, &status, &linuxTimeout);
    return res != 0;  // checks thread state
#endif
}

inline void COsUtilities::sleep(unsigned long milliSeconds) {
    // if we want to sleep for more than 1 second - we need to call sleep
    // because usleep is limited to up to 1,000,000 microseconds.
    if (milliSeconds >= 1000) {
        ::sleep(milliSeconds / 1000);  // seconds
        if (milliSeconds % 1000) {
            ::usleep((milliSeconds % 1000) * 1000);
        }
    } else {
        ::usleep(milliSeconds * 1000);
    }
}

inline void* COsUtilities::LoadTheLibrary(const std::string& name) {
    return (void*)dlopen(name.c_str(), RTLD_LAZY);
}

inline int COsUtilities::FreeTheLibrary(void* iLibHandle) {
    return dlclose(iLibHandle);
}

inline void* COsUtilities::GetFunctionEntryPoint(void* iLibHandle,
                                                 const std::string& iFuncName) {
    return (void*)dlsym(iLibHandle, iFuncName.c_str());
}

inline std::string COsUtilities::GetLastErrorText() {
    std::string msg;
    const char* zErrorString = ::dlerror();
    if (zErrorString) {
        msg = zErrorString;
    }

    return msg;
}

inline std::string COsUtilities::GetRoamingUserPath() {
    return GetLocalUserPath();
}

inline void* COsUtilities::AllocteBuffer(unsigned int iSize,
                                         unsigned int iAlignment) {
    char* buffer;
    if (iAlignment == 1) {
#if defined(__arm__) || defined(__aarch64__)
        if (InuCommon::COsUtilities::SSE4IsSupported(iSize)) {
            iAlignment = 16;
        }
#else
        if (InuCommon::COsUtilities::AVXIsSupported(iSize)) {
            iAlignment = AVX_PADDING;
        } else if (InuCommon::COsUtilities::SSE4IsSupported(iSize)) {
            iAlignment = AVX_PADDING / 2;
        }
#endif
    }

    if (iAlignment < sizeof(void*)) {
        iAlignment = sizeof(void*);
    }

    posix_memalign((void**)&buffer, iAlignment, iSize);

    return buffer;
}

inline void COsUtilities::FreeBuffer(void* buffer, unsigned int iSize) {
    FreeBuffer(buffer);
}

inline void COsUtilities::FreeBuffer(void* buffer) {
    if (buffer) {
        free(buffer);
    }
}
}  // namespace InuCommon

#endif

#endif
