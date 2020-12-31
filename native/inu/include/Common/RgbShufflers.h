#ifndef __RGBSHUFFLERS_H__
#define __RGBSHUFFLERS_H__

#if defined(__ARM_NEON__) || defined(__aarch64__)
// SSE2 macros for Android/ARM/NEON
#include <SSE2NEON.h>
#elif defined(_WIN32)
// SSE2 intrinsics for Visual Studio
#include <intrin.h>
#elif defined(__linux__) && !defined(__arm__)
// SSE2 intrinsics for Linux
#include <x86intrin.h>
#elif defined(__linux__)
//#error No SSE2 Intrinsics defined.
#endif

namespace  InuCommon
{

    //////////////////////////////////////////////////////////////////////////
    // Pixel byte shufflers

    //RGBA
    struct CRgbaShuffler
    {
        static inline CTiffFile::ETiffFormat FormatType() { return CTiffFile::eRGBA; }
#if  defined(__ARM_NEON__) || defined(__aarch64__) || defined(WIN32) || defined(__SSE2__)
        //SSE
        static inline __m128i UnpackLowFirstRow(const __m128i& R,const __m128i& G,const __m128i& B,const __m128i& A)
        {
            return _mm_unpacklo_epi32(G, R);
        }

        static inline __m128i UnpackLowFirstSecondRow(const __m128i& R,const __m128i& G,const __m128i& B,const __m128i& A)
        {
            return _mm_unpacklo_epi32(B, A);
        }

        static inline __m128i UnpackHighFirstRow(const __m128i& R,const __m128i& G,const __m128i& B,const __m128i& A)
        {
            return _mm_unpackhi_epi32(G, R);
        }

        static inline __m128i UnpackHighFirstSecondRow(const __m128i& R,const __m128i& G,const __m128i& B,const __m128i& A)
        {
            return _mm_unpackhi_epi32(B, A);
        }
#endif
        static inline void InsertNaive(unsigned char*& buffer, const unsigned char R,const unsigned char G,const unsigned char B,const unsigned char A)
        {
            *buffer++     = R;
            *buffer++     = G;
            *buffer++     = B;
            *buffer++     = A;
        }
    };

    //BGRA
    struct CBgraShuffler
    {
    public:
        static inline CTiffFile::ETiffFormat FormatType() { return CTiffFile::eBGRA; }
#if  defined(__ARM_NEON__) || defined(__aarch64__) || defined(WIN32) || defined(__SSE2__)
        //SSE
        static inline __m128i UnpackLowFirstRow(const __m128i& R,const __m128i& G,const __m128i& B,const __m128i& A)
        {
            return _mm_unpacklo_epi32(B, R);
        }

        static inline __m128i UnpackLowFirstSecondRow(const __m128i& R,const __m128i& G,const __m128i& B,const __m128i& A)
        {
            return _mm_unpacklo_epi32(G, A);
        }


        static inline __m128i UnpackHighFirstRow(const __m128i& R,const __m128i& G,const __m128i& B,const __m128i& A)
        {
            return _mm_unpackhi_epi32(B, R);
        }

        static inline __m128i UnpackHighFirstSecondRow(const __m128i& R,const __m128i& G,const __m128i& B,const __m128i& A)
        {
            return _mm_unpackhi_epi32(G, A);
        }
#endif
        static inline void InsertNaive(unsigned char*& buffer, const unsigned char R,const unsigned char G,const unsigned char B,const unsigned char A)
        {
            *buffer++     = B;
            *buffer++     = G;
            *buffer++     = R;
            *buffer++     = A;
        }
    };

    //ABGR
    struct CAbgrShuffler
    {
    public:
        static inline CTiffFile::ETiffFormat FormatType() { return CTiffFile::eABGR; }
#if  defined(__ARM_NEON__) || defined(__aarch64__) || defined(WIN32) || defined(__SSE2__)
        //SSE
        static inline __m128i UnpackLowFirstRow(const __m128i& R,const __m128i& G,const __m128i& B,const __m128i& A)
        {
            return _mm_unpacklo_epi32(A, G);
        }

        static inline __m128i UnpackLowFirstSecondRow(const __m128i& R,const __m128i& G,const __m128i& B,const __m128i& A)
        {
            return _mm_unpacklo_epi32(B, R);
        }


        static inline __m128i UnpackHighFirstRow(const __m128i& R,const __m128i& G,const __m128i& B,const __m128i& A)
        {
            return _mm_unpackhi_epi32(A, G);
        }

        static inline __m128i UnpackHighFirstSecondRow(const __m128i& R,const __m128i& G,const __m128i& B,const __m128i& A)
        {
            return _mm_unpackhi_epi32(B, R);
        }
#endif
        static inline void InsertNaive(unsigned char*& buffer, const unsigned char R,const unsigned char G,const unsigned char B,const unsigned char A)
        {
            *buffer++     = A;
            *buffer++     = B;
            *buffer++     = G;
            *buffer++     = R;
        }
    };

    //ARGB
    struct CArgbShuffler
    {
    public:
        static inline CTiffFile::ETiffFormat FormatType() { return CTiffFile::eARGB; }
#if  defined(__ARM_NEON__) || defined(__aarch64__) || defined(WIN32) || defined(__SSE2__)
        //SSE
        static inline __m128i UnpackLowFirstRow(const __m128i& R,const __m128i& G,const __m128i& B,const __m128i& A)
        {
            return _mm_unpacklo_epi32(A, G);
        }

        static inline __m128i UnpackLowFirstSecondRow(const __m128i& R,const __m128i& G,const __m128i& B,const __m128i& A)
        {
            return _mm_unpacklo_epi32(R, B);
        }


        static inline __m128i UnpackHighFirstRow(const __m128i& R,const __m128i& G,const __m128i& B,const __m128i& A)
        {
            return _mm_unpackhi_epi32(A, G);
        }

        static inline __m128i UnpackHighFirstSecondRow(const __m128i& R,const __m128i& G,const __m128i& B,const __m128i& A)
        {
            return _mm_unpackhi_epi32(R, B);
        }
#endif
        static inline void InsertNaive(unsigned char*& buffer, const unsigned char R,const unsigned char G,const unsigned char B,const unsigned char A)
        {
            *buffer++     = A;
            *buffer++     = R;
            *buffer++     = G;
            *buffer++     = B;
        }
    };

}

#endif

