
#ifndef __YUV2RGB_H__
#define __YUV2RGB_H__

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

#include "TiffFile.h"
#include "RgbShufflers.h"
#include "OSUtilities.h"
#include "CommonAlgo.h"

#define YUV2RGB_MAT_0_0   ((short int)1192)
#define YUV2RGB_MAT_0_1   ((short int)0)
#define YUV2RGB_MAT_0_2   ((short int)1634)
#define YUV2RGB_MAT_0_3   ((int)-228260)
#define YUV2RGB_MAT_1_0   ((short int)1192)
#define YUV2RGB_MAT_1_1   ((short int)-401)
#define YUV2RGB_MAT_1_2   ((short int)-833)
#define YUV2RGB_MAT_1_3   ((int)138830)
#define YUV2RGB_MAT_2_0   ((short int)1192)
#define YUV2RGB_MAT_2_1   ((short int)2065)
#define YUV2RGB_MAT_2_2   ((short int)0 )
#define YUV2RGB_MAT_2_3   ((int)-283480)


namespace  InuCommon
{

    ////////////////////////////////////////////////////////////////////////
    /// \brief Non-interlaced filter, extracts only a left image
    ///
    /// Class: CYUV2RGB
    ///
    /// Role: Extract non-interlaced image
    ///
    /// Inherits: CFilter
    ///
    /// Responsibilities:
    ///      1. Used in non-interlaced mode, translates single raw YUV image to RGB
    ///      2.
    ///
    /// Comments:
    ///
    ///  Using templated policies to decide the  R, G, B, A sequence.
    ///  Should give better performance in this performance sensitive code (compile time function choosing)
    ////////////////////////////////////////////////////////////////////////

    template <typename RGBShufflerType>
    class CYUV2RGB
    {

    public:

        static inline void                 Convert2Rows(unsigned char*& pRGBFirst,              // Output buffer (first line)
            unsigned char*& pRGBSecond,             // Output buffer (second line)
            const unsigned short*& dataArrayFirst,  // input buffer (first line)
            const unsigned short*& dataArraySecond, // input buffer (second line)
            unsigned int width,                     // Image width
            unsigned short shiftY);                 // How many bits to shift in order to get the Y value

        static inline void                 Convert2RowsSSE(unsigned char*& pRGBFirst,              // Output buffer (first line)
            unsigned char*& pRGBSecond,             // Output buffer (second line)
            const unsigned short*& dataArrayFirst,  // input buffer (first line)
            const unsigned short*& dataArraySecond, // input buffer (second line)
            unsigned int width,                     // Image width
            unsigned short shiftY);                 // How many bits to shift in order to get the Y value
    };

    //////////////////////////////////////////////////////////////////////
    //                      INLINE FUNCTIONS                            //
    //////////////////////////////////////////////////////////////////////
    template <typename RGBShufflerType>
    static inline void YUV2RGB(unsigned char*& pRGB, unsigned short Y, unsigned char U, unsigned char V)
    {
        //Don't need to multiply with 0 ... it is 0...
        int r = Y*YUV2RGB_MAT_0_0 + /*U*YUV2RGB_MAT[0][1] +*/ V*YUV2RGB_MAT_0_2 + YUV2RGB_MAT_0_3;
        int g = Y*YUV2RGB_MAT_1_0 + U*YUV2RGB_MAT_1_1 + V*YUV2RGB_MAT_1_2 + YUV2RGB_MAT_1_3;
        int b = Y*YUV2RGB_MAT_2_0 + U*YUV2RGB_MAT_2_1 +/* V*YUV2RGB_MAT[2][2] +*/ YUV2RGB_MAT_2_3;

        RGBShufflerType::InsertNaive(pRGB,
            (unsigned char)( (InuCommon::COsUtilities::maxNoBranch(0, InuCommon::COsUtilities::minNoBranch(r, 262143))) >>10 ),
            (unsigned char)( (InuCommon::COsUtilities::maxNoBranch(0, InuCommon::COsUtilities::minNoBranch(g, 262143))) >>10 ),
            (unsigned char)( (InuCommon::COsUtilities::maxNoBranch(0, InuCommon::COsUtilities::minNoBranch(b, 262143))) >>10 ),
            0);
    }

    template <typename RGBShufflerType>
    inline void   CYUV2RGB<RGBShufflerType>::Convert2Rows(unsigned char*& pRGBFirst, unsigned char*& pRGBSecond,
        const unsigned short*& dataArrayFirst, const unsigned short*& dataArraySecond,
        unsigned int width, unsigned short shiftY)
    {
        //Convert to RGB in target image
        for (unsigned  int x = 0 ; x < width; x+=2 )
        {
            const unsigned short& f0 = *dataArrayFirst++ ;
            const unsigned short& f1 = *dataArrayFirst++ ;
            const unsigned short& s0 = *dataArraySecond++ ;
            const unsigned short& s1 = *dataArraySecond++ ;

            unsigned char U(((f0 & 0xF000u) >> 8) | ((s0 & 0xF000u) >> 12));
            unsigned char V(((f1 & 0xF000u) >> 8) | ((s1 & 0xF000u) >> 12));

            YUV2RGB<RGBShufflerType>(pRGBFirst ,(f0 & 0x0FFFu) >> shiftY, U, V);
            YUV2RGB<RGBShufflerType>(pRGBFirst, (f1 & 0x0FFFu) >> shiftY, U, V);

            YUV2RGB<RGBShufflerType>(pRGBSecond, (s0 & 0x0FFFu) >> shiftY, U, V);
            YUV2RGB<RGBShufflerType>(pRGBSecond, (s1 & 0x0FFFu) >> shiftY, U, V);
        }
    }

#if defined(__aarch64__) || defined(__ARM_NEON__) || defined(_MSC_VER) || defined(__SSE2__)
    inline __m128i OneColor(const __m128i& Y, const __m128i& U, const __m128i& V, const __m128i& mat0, const __m128i& mat1, const __m128i& mat2,  const __m128i& mat3)
    {
        const __m128i& zero = _mm_set1_epi32(0);
        static const __m128i& maxLimit = _mm_set1_epi32(262143);

        __m128i result = _mm_mullo_epi32( Y,  mat0);
        __m128i tmp = _mm_mullo_epi32( U, mat1);
        result = _mm_add_epi32(result, tmp);
        tmp = _mm_mullo_epi32( V, mat2);
        result = _mm_add_epi32(result, tmp);
        result = _mm_add_epi32(result, mat3);

        result = _mm_max_epi32(zero, result);
        result = _mm_min_epi32(maxLimit, result);
        result = _mm_srli_epi32(result, 10); // shift right logical by 10

        return result;
    }

    template <typename RGBShufflerType>
    inline  void   CYUV2RGB<RGBShufflerType>::Convert2RowsSSE(unsigned char*& pRGBFirst, unsigned char*& pRGBSecond,
        const unsigned short*& dataArrayFirst, const unsigned short*& dataArraySecond,
        unsigned int width, unsigned short shiftY)
    {
        static const __m128i  mat00=_mm_set1_epi32(YUV2RGB_MAT_0_0);
        static const __m128i  mat01=_mm_set1_epi32(YUV2RGB_MAT_0_1);
        static const __m128i  mat02=_mm_set1_epi32(YUV2RGB_MAT_0_2);
        static const __m128i  mat03=_mm_set1_epi32(YUV2RGB_MAT_0_3);

        static const __m128i  mat10=_mm_set1_epi32(YUV2RGB_MAT_1_0);
        static const __m128i  mat11=_mm_set1_epi32(YUV2RGB_MAT_1_1);
        static const __m128i  mat12=_mm_set1_epi32(YUV2RGB_MAT_1_2);
        static const __m128i  mat13=_mm_set1_epi32(YUV2RGB_MAT_1_3);

        static const __m128i  mat20=_mm_set1_epi32(YUV2RGB_MAT_2_0);
        static const __m128i  mat21=_mm_set1_epi32(YUV2RGB_MAT_2_1);
        static const __m128i  mat22=_mm_set1_epi32(YUV2RGB_MAT_2_2);
        static const __m128i  mat23=_mm_set1_epi32(YUV2RGB_MAT_2_3);
        static const __m128i  maskF=_mm_set1_epi16(0xF000u);

        const __m128i zero = _mm_set1_epi32(0);
        __m128i* pRGBFirst128 = (__m128i*)pRGBFirst;
        __m128i* pRGBSecond128 = (__m128i*)pRGBSecond;
        __m128i* dataArrayFirst128 = (__m128i*)dataArrayFirst;
        __m128i* dataArraySecond128 = (__m128i*)dataArraySecond;

        for (unsigned int x = 0; x < width; x+=8)
        {
            // Working on 4 pixels (2 X 2 pairs)

            __m128i upper = _mm_load_si128 (dataArrayFirst128++); // unaligned  -  load  data [0,...,7]
            __m128i lower = _mm_load_si128 (dataArraySecond128++); // unaligned

            __m128i upperUV = _mm_and_si128(maskF, upper); // mask with 0xf000
            upperUV = _mm_srli_epi16(upperUV, 8); // shift right logical by 8

            __m128i lowerUV  = _mm_and_si128(maskF, lower); // mask with 0xf000
            lowerUV = _mm_srli_epi16(lowerUV, 12); // shift right logical by 12

            __m128i uv32b = _mm_or_si128(upperUV, lowerUV);   // { U_0, V_0, U_1, V_1,  U_2, V_2, U_3, V_3}
            __m128i uv32bLow = _mm_unpacklo_epi16(uv32b, zero);          // { U_0, V_0, U_1, V_1}
            __m128i u32low = _mm_shuffle_epi32(uv32bLow, _MM_SHUFFLE(2,2,0,0));  // { U_0, U_0, U_1, U_1}
            __m128i v32low = _mm_shuffle_epi32(uv32bLow, _MM_SHUFFLE(3,3,1,1));  // { V_0, V_0, v_1, V_1}

            __m128i uv32bHi = _mm_unpackhi_epi16(uv32b, zero);          // { U_2, V_2, U_3, V_3}
            __m128i u32hi = _mm_shuffle_epi32(uv32bHi, _MM_SHUFFLE(2,2,0,0));  // { U_2, U_2, U_3, U_3}
            __m128i v32hi = _mm_shuffle_epi32(uv32bHi, _MM_SHUFFLE(3,3,1,1));  // { V_2, V_2, v_3, V_3}

            {
                __m128i Y = _mm_andnot_si128(maskF, upper); // mask with 0xfff
                __m128i Y32 = _mm_srli_epi16(Y, shiftY);    // { Y0_0, Y1_0, Y2_0, Y3_0, Y4_0, Y5_0, Y6_0, Y7_0 }

                {
                    __m128i Y32Low = _mm_unpacklo_epi16(Y32, zero);   // { Y0_0, Y1_0, Y2_0, Y3_0}

                    __m128i  R = OneColor(Y32Low, u32low, v32low, mat00, mat01, mat02, mat03);
                    __m128i  G = OneColor(Y32Low, u32low, v32low, mat10, mat11, mat12, mat13);
                    __m128i  B = OneColor(Y32Low, u32low, v32low, mat20, mat21, mat22, mat23);

#if  !defined(__arm__) && !defined(__aarch64__)
                    _mm_prefetch((char*)pRGBFirst128 ,_MM_HINT_T0 );
#endif

                    __m128i tmp1 = RGBShufflerType::UnpackLowFirstRow(R, G, B, zero);
                    __m128i tmp2 = RGBShufflerType::UnpackLowFirstSecondRow(R, G, B, zero);

                    __m128i pixel1 = _mm_unpacklo_epi32(tmp1,tmp2);
                    __m128i pixel2 = _mm_unpackhi_epi32(tmp1,tmp2);
                    __m128i pixel12 = _mm_packs_epi32(pixel1, pixel2);

                    tmp1 = RGBShufflerType::UnpackHighFirstRow(R, G, B, zero);
                    tmp2 = RGBShufflerType::UnpackHighFirstSecondRow(R, G, B, zero);

                    __m128i pixel3 = _mm_unpacklo_epi32(tmp1,tmp2);
                    __m128i pixel4 = _mm_unpackhi_epi32(tmp1,tmp2);
                    __m128i pixel34 = _mm_packs_epi32(pixel3, pixel4);

                    __m128i result = _mm_packus_epi16(pixel12, pixel34);

                    _mm_storeu_si128 (pRGBFirst128++ , result);
                }

                {
                    __m128i Y32Hi = _mm_unpackhi_epi16(Y32, zero);   // { Y4_0, Y5_0, Y6_0, Y7_0}

                    __m128i  R = OneColor(Y32Hi, u32hi, v32hi, mat00, mat01, mat02, mat03);
                    __m128i  G = OneColor(Y32Hi, u32hi, v32hi, mat10, mat11, mat12, mat13);
                    __m128i  B = OneColor(Y32Hi, u32hi, v32hi, mat20, mat21, mat22, mat23);

#if  !defined(__arm__) && !defined(__aarch64__)
                    _mm_prefetch((char*)pRGBFirst128 ,_MM_HINT_T0 );
#endif

                    __m128i tmp1 = RGBShufflerType::UnpackLowFirstRow(R, G, B, zero);
                    __m128i tmp2 = RGBShufflerType::UnpackLowFirstSecondRow(R, G, B, zero);

                    __m128i pixel1 = _mm_unpacklo_epi32(tmp1,tmp2);
                    __m128i pixel2 = _mm_unpackhi_epi32(tmp1,tmp2);
                    __m128i pixel12 = _mm_packs_epi32(pixel1, pixel2);


                    tmp1 = RGBShufflerType::UnpackHighFirstRow(R, G, B, zero);
                    tmp2 = RGBShufflerType::UnpackHighFirstSecondRow(R, G, B, zero);

                    __m128i pixel3 = _mm_unpacklo_epi32(tmp1,tmp2);
                    __m128i pixel4 = _mm_unpackhi_epi32(tmp1,tmp2);
                    __m128i pixel34 = _mm_packs_epi32(pixel3, pixel4);

                    __m128i result = _mm_packus_epi16(pixel12, pixel34);

                    _mm_storeu_si128 (pRGBFirst128++ , result);
                }
            }

            {
                __m128i Y = _mm_andnot_si128(maskF, lower); // mask with 0xfff
                __m128i Y32 = _mm_srli_epi16(Y, shiftY);    // { Y0_0, Y1_0, Y2_0, Y3_0, Y4_0, Y5_0, Y6_0, Y7_0 }

                {
                    __m128i Y32Low = _mm_unpacklo_epi16(Y32, zero);   // { Y0_0, Y1_0, Y2_0, Y3_0}

                    __m128i  R = OneColor(Y32Low, u32low, v32low, mat00, mat01, mat02, mat03);
                    __m128i  G = OneColor(Y32Low, u32low, v32low, mat10, mat11, mat12, mat13);
                    __m128i  B = OneColor(Y32Low, u32low, v32low, mat20, mat21, mat22, mat23);

#if  !defined(__arm__) && !defined(__aarch64__)
                    _mm_prefetch((char*) pRGBSecond128 ,_MM_HINT_T0 );
#endif


                    __m128i tmp1 = RGBShufflerType::UnpackLowFirstRow(R, G, B, zero);
                    __m128i tmp2 = RGBShufflerType::UnpackLowFirstSecondRow(R, G, B, zero);

                    __m128i pixel1 = _mm_unpacklo_epi32(tmp1,tmp2);
                    __m128i pixel2 = _mm_unpackhi_epi32(tmp1,tmp2);
                    __m128i pixel12 = _mm_packs_epi32(pixel1, pixel2);

                    tmp1 = RGBShufflerType::UnpackHighFirstRow(R, G, B, zero);
                    tmp2 = RGBShufflerType::UnpackHighFirstSecondRow(R, G, B, zero);

                    __m128i pixel3 = _mm_unpacklo_epi32(tmp1,tmp2);
                    __m128i pixel4 = _mm_unpackhi_epi32(tmp1,tmp2);
                    __m128i pixel34 = _mm_packs_epi32(pixel3, pixel4);

                    __m128i result = _mm_packus_epi16(pixel12, pixel34);

                    _mm_storeu_si128 (pRGBSecond128++ , result);
                }

                {
                    __m128i Y32Hi = _mm_unpackhi_epi16(Y32, zero);   // { Y4_0, Y5_0, Y6_0, Y7_0}

                    __m128i  R = OneColor(Y32Hi, u32hi, v32hi, mat00, mat01, mat02, mat03);
                    __m128i  G = OneColor(Y32Hi, u32hi, v32hi, mat10, mat11, mat12, mat13);
                    __m128i  B = OneColor(Y32Hi, u32hi, v32hi, mat20, mat21, mat22, mat23);

#if  !defined(__arm__) && !defined(__aarch64__)
                    _mm_prefetch((char*) pRGBSecond128 ,_MM_HINT_T0 );
#endif

                    __m128i tmp1 = RGBShufflerType::UnpackLowFirstRow(R, G, B, zero);
                    __m128i tmp2 = RGBShufflerType::UnpackLowFirstSecondRow(R, G, B, zero);

                    __m128i pixel1 = _mm_unpacklo_epi32(tmp1,tmp2);
                    __m128i pixel2 = _mm_unpackhi_epi32(tmp1,tmp2);
                    __m128i pixel12 = _mm_packs_epi32(pixel1, pixel2);

                    tmp1 = RGBShufflerType::UnpackHighFirstRow(R, G, B, zero);
                    tmp2 = RGBShufflerType::UnpackHighFirstSecondRow(R, G, B, zero);

                    __m128i pixel3 = _mm_unpacklo_epi32(tmp1,tmp2);
                    __m128i pixel4 = _mm_unpackhi_epi32(tmp1,tmp2);
                    __m128i pixel34 = _mm_packs_epi32(pixel3, pixel4);

                    __m128i result = _mm_packus_epi16(pixel12, pixel34);

                    _mm_storeu_si128 (pRGBSecond128++ , result);
                }
            }
        }

        pRGBFirst = (unsigned char*)pRGBFirst128;
        pRGBSecond = (unsigned char*)pRGBSecond128;
        dataArrayFirst = (unsigned short*)dataArrayFirst128;
        dataArraySecond = (unsigned short*)dataArraySecond128;
    }
#endif
}

#endif

