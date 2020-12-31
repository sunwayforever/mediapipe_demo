//
// File : OSUtilitiess.h
//

#ifndef __OSUITLITIESVS_H__
#define __OSUITLITIESVS_H__


#ifdef _WIN32

#include <smmintrin.h>
#include <bitset>
#include <xlocbuf>
#include <codecvt>

#include <intrin.h>

#if _MSC_VER == 1600
#define FOR_EACH(var , range) for each(var in range)
#else
#define FOR_EACH(var , range) for (var : range)
#endif

namespace InuCommon {

#if (defined(_UNICODE) || defined(UNICODE))
    //convert UTF8 to UTF16
    inline std::wstring COsUtilities::ConvertEncoding(const std::string& iFilename)
    {
        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> convert;
        return convert.from_bytes(iFilename);
    }

    inline std::string COsUtilities::ConvertEncodingBack(const std::wstring& iFilename)
    {
        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> convert;
        return convert.to_bytes(iFilename);
    }
#endif

    inline bool COsUtilities::IsSessionZeroIsolate()
    {
        //   unsigned long major=0; unsigned long minor=0; unsigned long productType=0;
        //   unsigned long architecture=0; unsigned long platformId=0;
        //   GetOSVersionInfo(major, minor, productType, architecture, platformId);

        //   // osvi.dwMajorVersion >=6 ==> 2008, Vista, windows7
        //if (major >= 6)
        // return true;
        //else
        // return false;
        return true;
    }

    inline void  COsUtilities::memcpy(void* dest, size_t destSize, const void* src, size_t count)
    {
        memcpy_s(dest, destSize, src, count);
    }

    inline void  COsUtilities::strcpy(char* dest, size_t destSize, const char* src)
    {
        strcpy_s(dest, destSize, src);
    }

    inline void  COsUtilities::strncpy(char* dest, size_t destSize, const char* src, size_t count)
    {
        strncpy_s(dest, destSize, src, count);
    }

    inline std::string COsUtilities::getenv(const std::string& varname)
    {
        static const int BUFFER_SIZE=1024;
        size_t len=BUFFER_SIZE;
        wchar_t tmp[BUFFER_SIZE];
        _wgetenv_s(&len, tmp, InuCommon::COsUtilities::ConvertEncoding(varname).c_str());

        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> convert;
        return convert.to_bytes(std::wstring(tmp, len > 0 ? len-1 : 0));
    }

    inline void  COsUtilities::fopen(FILE*& file, const char * filename, const char * mode)
    {
        std::string utf8Name(filename);

        if (_wfopen_s(&file, ConvertEncoding(filename).c_str(), ConvertEncoding(mode).c_str()))
            file = NULL;
    }


    inline void COsUtilities::sleep(unsigned long milliSeconds)
    {
        ::Sleep(milliSeconds);
    }

    inline void* COsUtilities::CreateThread(THREAD_FUNC threadFunc, void * params)
    {
        DWORD threadId = 0;
        HANDLE ret=::CreateThread(0, 0, (LPTHREAD_START_ROUTINE)threadFunc,  params, 0, &threadId);
        return ret;
    }

    inline uint32_t COsUtilities::ProcessID()
    {
        return ::GetCurrentProcessId();
    }

    inline std::string COsUtilities::GetExeFullPathFileName()
    {
        std::string result("");
        char tmp[MAX_PATH];
        DWORD size=GetModuleFileNameA(NULL, tmp, MAX_PATH);
        if (size != -1)
        {
             result = std::string(tmp, size);
        }
        return result;
    }

    inline int32_t COsUtilities::ThreadID()
    {
        return ::GetCurrentThreadId();
    }

    inline long COsUtilities::FetchAndIncrement(long * pValue)
    {
        return ::InterlockedIncrement(pValue);
    }

    inline long COsUtilities::FetchAndDecrement(long * pValue)
    {
        return ::InterlockedDecrement(pValue);
    }

    inline long COsUtilities::CompareAndExchange(volatile long * pValue, long newValue, long comparand)
    {
        return ::InterlockedCompareExchange(pValue, newValue, comparand);
    }

    inline void* COsUtilities::LoadTheLibrary(const std::string& name)
    {
        return (void*)::LoadLibrary(COsUtilities::ConvertEncoding(name).c_str());
    }

	inline int COsUtilities::FreeTheLibrary(void* iLibHandle)
	{
		if (!FreeLibrary((HMODULE)iLibHandle))
		{
			return GetLastError();
		}

		return 0; // success
		return (FreeLibrary((HMODULE)iLibHandle)) ? 0 : 1;   // 0 for success, 1 for error
	}

    inline std::string COsUtilities::GetLastErrorText()
    {
        DWORD errorCode = ::GetLastError();
        if (errorCode != 0)
        {
            LPSTR buffer = nullptr;
            size_t size = FormatMessageA( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                NULL, errorCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&buffer, 0, NULL);

            std::string msg(buffer, size);

            //Free the buffer.
            LocalFree(buffer);

            return msg;
        }
        
        return std::string(); 
    }

	
	inline void* COsUtilities::GetFunctionEntryPoint(void* iLibHandle, const std::string& iFuncName)
    {
        return (void*)::GetProcAddress((HMODULE)iLibHandle, iFuncName.c_str());
    }

    inline bool COsUtilities::FileDelete(const std::string& name)
    {
        try {
            // check if the directory already exists
            return (::DeleteFile((LPCTSTR)name.c_str()) == TRUE);
        }
        catch (...) {
            return false;
        }
    }
    // Time

    inline bool COsUtilities::SSE4IsSupported(unsigned int iWidth, EOptimizationLevel iRequestedLevel)
    {
        if ( (iWidth % 16 != 0) || (iRequestedLevel < eSSE) )
        {
            return false;
        }

        static int IsSSE4Supported(-1);
        if (IsSSE4Supported == -1)
        {
            int CPUInfo[4];
            __cpuid(CPUInfo , 1);

            // SSE4.1, (bit 20 is for SSE4.2)
            std::bitset<32> SSE4_MASK;
            SSE4_MASK.reset();
            SSE4_MASK.set(19);

            IsSSE4Supported = (unsigned long(CPUInfo[2]) & SSE4_MASK.to_ulong());
        }
        return (IsSSE4Supported != 0);
    }

    inline bool COsUtilities::AVXIsSupported(unsigned int iWidth, EOptimizationLevel iRequestedLevel)
    {
        if ( (iWidth % AVX_PADDING != 0) || (iRequestedLevel < eAVX) )
        {
            return false;
        }
        
        static int IsAVXSupported(-1);
        if (IsAVXSupported == -1)
        {
            int CPUInfo[4];
            __cpuidex(CPUInfo , 7, 0);

            std::bitset<32> AVX2_MASK;
            AVX2_MASK.reset();
            AVX2_MASK.set(5);

            IsAVXSupported =  (unsigned long(CPUInfo[1]) & AVX2_MASK.to_ulong());
        }

        return (IsAVXSupported != 0);
    }

    EXTERN_C IMAGE_DOS_HEADER __ImageBase;

    inline std::string COsUtilities::GetDLLFullPathFileName()
    {
        return COsUtilities::GetDLLFileNameImp((HINSTANCE) &__ImageBase);
    }

    inline std::string COsUtilities::GetDLLFullPathFileName(const std::string& dllName)
    {
        HANDLE tmp = GetModuleHandle(ConvertEncoding(dllName).c_str());
        return COsUtilities::GetDLLFileNameImp((HINSTANCE) tmp);
    }

    inline void* COsUtilities::AllocteBuffer(unsigned int iSize, unsigned int iAlignement)
    {
        char* buffer;
        if (iAlignement == 1)
        {
            if (InuCommon::COsUtilities::AVXIsSupported(iSize))
            {
                iAlignement = AVX_PADDING;
            }
            else if (InuCommon::COsUtilities::SSE4IsSupported(iSize))
            {
                iAlignement = AVX_PADDING / 2;
            }
        }

        buffer = (char*)_mm_malloc(iSize, iAlignement);

        return buffer;
    }

    inline void COsUtilities::FreeBuffer(void* buffer, unsigned int iSize)
    {
        _mm_free(buffer);
    }

    inline void COsUtilities::FreeBuffer(void* buffer)
    {
        FreeBuffer(buffer, 128);
    }



}

#endif
#endif
