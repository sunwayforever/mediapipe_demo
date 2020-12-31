#ifndef __ALGOAVX__
#define __ALGOAVX__

#include "YUV2RGB.h"
#include "CommonAlgo.h"
#include "AlgoDepthOpticalData.h"

namespace  InuCommon
{
    ////////////////////////////////////////////////////////////////////////
    /// \brief    Utility class for basic algorithms
    ///
    /// Responsibilities:
    ///      1. Knows how to convert disparity image to depth map.
    ////////////////////////////////////////////////////////////////////////
    class COMMONALGO_API CAlgoAvx
    {
    public:


        /// \brief    Convert input disparity image into depth map.
        /// \param[out] outputBuffer    Output depth map (Z-Buffer).
        /// \param[in] inputBuffer    Input disparity image.
        /// \param[in] inputBufferSize    Input disparity buffer size.
        /// \param[in] opticalData    Optical data which is used for calculation.
        /// \param[in] confidenceThreshold    Minimum confidence.
        /// \param[in] maxdepth    Maximum depth value in output buffer.
        /// \param[in] nonValidDepthValue    Non valid depth value.
        /// \param[in] iRequestedLevel    Overwrites optimization level that is supported by the host machine.
        /// \return  number of non valid depth pixels
        static uint32_t Disparity2Depth(unsigned short* outputBuffer,
                                    const unsigned short* inputBuffer,
                                    unsigned int inputBufferSize,
                                    const CDepthOpticalData& opticalData,
                                    unsigned short disparityMask,
                                    unsigned short disparityShift,
                                    unsigned short disparityMaxValue,
                                    float disparityDivider,
                                    unsigned short confidenceMask,
                                    unsigned short confidenceShift,
                                    unsigned int confidenceThreshold,
                                    unsigned short maxdepth,
                                    unsigned short nonValidDepthValue,
                                    uint32_t disparityFrameOffsetValue,
                                    unsigned short outputConfidenceShift,
                                    unsigned short outputDepthShift,
                                    float iScale);


        /// \brief    Disparity Improvement Algorithm (fast mode with video)
        /// \param[in] ioDisparity    Input disparity image that is improved.
        /// \param[in] iYimage    Input Y image.
        /// \param[in,out] iImageSize    Image size (width * height).
        static void DisparityImrovementFastMode(unsigned short* iDisparity,
            const unsigned short* iYimage,
            unsigned int iImageSize);
    };


    template <typename RGBShufflerType>
    class CYUV2RGBAvx
    {
    public:
        static void  Convert2RowsAVX(unsigned char*& pRGBFirst, unsigned char*& pRGBSecond,
            const unsigned short*& dataArrayFirst, const unsigned short*& dataArraySecond,
            unsigned int width, unsigned short shiftY);
    };

#if defined(__AVX2__) || defined(_MSC_VER)

    template <typename Shuffler>
    class GetShuffler {};

    //RGBA
    struct CRgbaShufflerAvx
    {
        //AVX
          static inline __m256i UnpackLowFirstRow(const __m256i& R,const __m256i& G,const __m256i& B,const __m256i& A)
          {
              return _mm256_unpacklo_epi32(G, R);
          }

          static inline __m256i UnpackLowFirstSecondRow(const __m256i& R,const __m256i& G,const __m256i& B,const __m256i& A)
          {
              return _mm256_unpacklo_epi32(B, A);
          }

          static inline __m256i UnpackHighFirstRow(const __m256i& R,const __m256i& G,const __m256i& B,const __m256i& A)
          {
              return _mm256_unpackhi_epi32(G, R);
          }

          static inline __m256i UnpackHighFirstSecondRow(const __m256i& R,const __m256i& G,const __m256i& B,const __m256i& A)
          {
              return _mm256_unpackhi_epi32(B, A);
          }
    };

    //RGBA
    struct CBgraShufflerAvx
    {
        //AVX
         static inline __m256i UnpackLowFirstRow(const __m256i& R,const __m256i& G,const __m256i& B,const __m256i& A)
         {
             return _mm256_unpacklo_epi32(B, R);
         }

         static inline __m256i UnpackLowFirstSecondRow(const __m256i& R,const __m256i& G,const __m256i& B,const __m256i& A)
         {
             return _mm256_unpacklo_epi32(G, A);
         }

         static inline __m256i UnpackHighFirstRow(const __m256i& R,const __m256i& G,const __m256i& B,const __m256i& A)
         {
             return _mm256_unpackhi_epi32(B, R);
         }

         static inline __m256i UnpackHighFirstSecondRow(const __m256i& R,const __m256i& G,const __m256i& B,const __m256i& A)
         {
             return _mm256_unpackhi_epi32(G, A);
         }
    };

    //ABGR
    struct CAbgrShufflerAvx
    {
        //AVX
         static inline __m256i UnpackLowFirstRow(const __m256i& R,const __m256i& G,const __m256i& B,const __m256i& A)
         {
             return _mm256_unpacklo_epi32(A, G);
         }

         static inline __m256i UnpackLowFirstSecondRow(const __m256i& R,const __m256i& G,const __m256i& B,const __m256i& A)
         {
             return _mm256_unpacklo_epi32(B, R);
         }

         static inline __m256i UnpackHighFirstRow(const __m256i& R,const __m256i& G,const __m256i& B,const __m256i& A)
         {
             return _mm256_unpackhi_epi32(A, G);
         }

         static inline __m256i UnpackHighFirstSecondRow(const __m256i& R,const __m256i& G,const __m256i& B,const __m256i& A)
         {
             return _mm256_unpackhi_epi32(B, R);
         }
    };

    //ARGB
    struct CArgbShufflerAvx
    {
        //AVX
          static inline __m256i UnpackLowFirstRow(const __m256i& R,const __m256i& G,const __m256i& B,const __m256i& A)
          {
              return _mm256_unpacklo_epi32(A, G);
          }

          static inline __m256i UnpackLowFirstSecondRow(const __m256i& R,const __m256i& G,const __m256i& B,const __m256i& A)
          {
              return _mm256_unpacklo_epi32(B, R);
          }

          static inline __m256i UnpackHighFirstRow(const __m256i& R,const __m256i& G,const __m256i& B,const __m256i& A)
          {
              return _mm256_unpackhi_epi32(A, G);
          }

          static inline __m256i UnpackHighFirstSecondRow(const __m256i& R,const __m256i& G,const __m256i& B,const __m256i& A)
          {
              return _mm256_unpackhi_epi32(B, R);
          }
    };


    inline __m256i OneColor256(const __m256i& Y, const __m256i& U, const __m256i& V, const __m256i& mat0, const __m256i& mat1, const __m256i& mat2,  const __m256i& mat3)
    {
        static const __m256i zero256 = _mm256_set1_epi32(0);
        static const __m256i maxLimit = _mm256_set1_epi32(262143);

        __m256i result = _mm256_mullo_epi32( Y, mat0);
        __m256i tmp    = _mm256_mullo_epi32( U, mat1);

        result = _mm256_add_epi32(result, tmp);
        tmp    = _mm256_mullo_epi32( V, mat2);
        result = _mm256_add_epi32(result, tmp);
        result = _mm256_add_epi32(result, mat3);

        result = _mm256_max_epi32(zero256, result);
        result = _mm256_min_epi32(maxLimit, result);
        result = _mm256_srli_epi32(result, 10); // shift right logical by 10

        return result;
    }

    template <typename RGBShufflerType>
    inline  void   CYUV2RGBAvx<RGBShufflerType>::Convert2RowsAVX(unsigned char*& pRGBFirst, unsigned char*& pRGBSecond,
        const unsigned short*& dataArrayFirst, const unsigned short*& dataArraySecond,
        unsigned int width, unsigned short shiftY)
    {
        static const __m256i  mat00_256 = _mm256_set1_epi32(YUV2RGB_MAT_0_0);
        static const __m256i  mat01_256 = _mm256_set1_epi32(YUV2RGB_MAT_0_1);
        static const __m256i  mat02_256 = _mm256_set1_epi32(YUV2RGB_MAT_0_2);
        static const __m256i  mat03_256 = _mm256_set1_epi32(YUV2RGB_MAT_0_3);

        static const __m256i  mat10_256 = _mm256_set1_epi32(YUV2RGB_MAT_1_0);
        static const __m256i  mat11_256 = _mm256_set1_epi32(YUV2RGB_MAT_1_1);
        static const __m256i  mat12_256 = _mm256_set1_epi32(YUV2RGB_MAT_1_2);
        static const __m256i  mat13_256 = _mm256_set1_epi32(YUV2RGB_MAT_1_3);

        static const __m256i  mat20_256 = _mm256_set1_epi32(YUV2RGB_MAT_2_0);
        static const __m256i  mat21_256 = _mm256_set1_epi32(YUV2RGB_MAT_2_1);
        static const __m256i  mat22_256 = _mm256_set1_epi32(YUV2RGB_MAT_2_2);
        static const __m256i  mat23_256 = _mm256_set1_epi32(YUV2RGB_MAT_2_3);
        static const __m256i  maskF256  = _mm256_set1_epi16(0xF000u);

        static const __m256i  uvShuffle = _mm256_set_epi32(0xFF0EFF0A, 0xFF06FF02, 0xFF0CFF08, 0xFF04FF00, 0xFF0EFF0A, 0xFF06FF02, 0xFF0CFF08, 0xFF04FF00);
        static const __m256i  uvPermLow = _mm256_set_epi32(3, 3, 2, 2, 1, 1, 0 ,0);
        static const __m256i  uvPermHi  = _mm256_set_epi32(7, 7, 6, 6, 5, 5, 4 ,4);

        static const __m256i zero256 = _mm256_set1_epi32(0);
        __m256i* dataArrayFirst256 = (__m256i*)dataArrayFirst;
        __m256i* dataArraySecond256 = (__m256i*)dataArraySecond;
        __m256i* pRGBFirst256 = (__m256i*)pRGBFirst;
        __m256i* pRGBSecond256 = (__m256i*)pRGBSecond;

        for (unsigned int x = 0; x < width; x += 16)
        {
            // each iteration processes: 16 upper Y, 16 lower Y, 8 common U, 8 common V

            __m256i upper   = _mm256_load_si256 (dataArrayFirst256++);
            __m256i lower   = _mm256_load_si256 (dataArraySecond256++);

            __m256i upperUV = _mm256_and_si256(maskF256, upper); // mask with 0xf000
            upperUV = _mm256_srli_epi16(upperUV, 8);     // shift right logical by 8

            __m256i lowerUV = _mm256_and_si256(maskF256, lower); // mask with 0xf000
            lowerUV = _mm256_srli_epi16(lowerUV, 12);    // shift right logical by 12

            __m256i uv16    = _mm256_or_si256(upperUV, lowerUV);   // { U_0, V_0, U_1, V_1, U_2, V_2, U_3, V_3, U_4, V_4, U_5, V_5, U_6, V_6, U_7, V_7 }
            uv16    = _mm256_shuffle_epi8(uv16, uvShuffle);// { U_0, U_1, U_2, U_3, V_0, V_1, V_2, V_3, U_4, U_5, U_6, U_7, V_4, V_5, V_6, V_7 }

            __m256i u32     = _mm256_unpacklo_epi16(uv16, zero256); // { U_0, U_1, U_2, U_3, U_4, U_5, U_6, U_7 }
            __m256i v32     = _mm256_unpackhi_epi16(uv16, zero256); // { V_0, V_1, V_2, V_3, V_4, V_5, V_6, V_7 }

            __m256i u32L    = _mm256_permutevar8x32_epi32(u32,uvPermLow); // { U_0, U_0, U_1, U_1, U_2, U_2, U_3, U_3 }
            __m256i u32H    = _mm256_permutevar8x32_epi32(u32,uvPermHi);  // { U_4, U_4, U_5, U_5, U_6, U_6, U_7, U_7 }

            __m256i v32L    = _mm256_permutevar8x32_epi32(v32,uvPermLow); // { V_0, V_0, V_1, V_1, V_2, V_2, V_3, V_3 }
            __m256i v32H    = _mm256_permutevar8x32_epi32(v32,uvPermHi);  // { V_4, V_4, V_5, V_5, V_6, V_6, V_7, V_7 }

            /* ---------- calc: upper row, low part (i.e. pels: 0 - 15 ) ---------- */
            __m256i Y    = _mm256_andnot_si256(maskF256, upper); // mask with 0xfff
            __m256i Y32  = _mm256_srli_epi16(Y, 4);              // { Y0_0, Y1_0, Y2_0, Y3_0, Y4_0, Y5_0, Y6_0, Y7_0, Y8_0, Y9_0, Y10_0, Y1_0, Y12_0, Y13_0, Y14_0, Y15_0 }
            Y32  = _mm256_permute4x64_epi64(Y32, _MM_SHUFFLE(3,1,2,0)); //{ Y0_0, Y1_0, Y2_0, Y3_0, Y8_0, Y9_0, Y10_0, Y11_0, Y4_0, Y5_0, Y6_0, Y7_0, Y12_0, Y13_0, Y14_0, Y15_0 }
            __m256i Y32L = _mm256_unpacklo_epi16(Y32, zero256);  // { Y0_0, Y1_0, Y2_0, Y3_0, Y4_0, Y5_0, Y6_0, Y7_0 }
            __m256i Y32H = _mm256_unpackhi_epi16(Y32, zero256);  // { Y8_0, Y9_0, Y10_0, Y11_0, Y12_0, Y13_0, Y14_0, Y15_0 }

            __m256i  R = OneColor256(Y32L, u32L, v32L, mat00_256, mat01_256, mat02_256, mat03_256);
            __m256i  G = OneColor256(Y32L, u32L, v32L, mat10_256, mat11_256, mat12_256, mat13_256);
            __m256i  B = OneColor256(Y32L, u32L, v32L, mat20_256, mat21_256, mat22_256, mat23_256);

            _mm_prefetch((char*)pRGBFirst256 ,_MM_HINT_T0 );

            __m256i tmp1    = GetShuffler<RGBShufflerType>::type::UnpackLowFirstRow(R, G, B, zero256);       // { B_0, R_0, B_1, R_1, B_4, R_4, B_5, R_5 }
            __m256i tmp2    = GetShuffler<RGBShufflerType>::type::UnpackLowFirstSecondRow(R, G, B, zero256); // { G_0, 0,   G_1, 0,   G_4, 0,   G_5, 0 }
            __m256i pixel1  = _mm256_unpacklo_epi32(tmp1,tmp2);  // { B_0, G_0, R_0, 0, B_4, G_4, R_4, 0 }
            __m256i pixel2  = _mm256_unpackhi_epi32(tmp1,tmp2);  // { B_1, G_1, R_1, 0, B_5, G_5, R_5, 0 }
            __m256i pixel12 = _mm256_packs_epi32(pixel1, pixel2);// { BGR0_0, BGR0_1, BGR0_4, BGR0_5}

            tmp1    = GetShuffler<RGBShufflerType>::type::UnpackHighFirstRow(R, G, B, zero256);       // { B_2, R_2, B_3, R_3, B_6, R_6, B_7, R_7 }
            tmp2    = GetShuffler<RGBShufflerType>::type::UnpackHighFirstSecondRow(R, G, B, zero256); // { G_2, 0,   G_3, 0,   G_6, 0,   G_7, 0 }
            __m256i pixel3  = _mm256_unpacklo_epi32(tmp1,tmp2);  // { B_2, G_2, R_2, 0, B_3, G_3, R_3, 0 }
            __m256i pixel4  = _mm256_unpackhi_epi32(tmp1,tmp2);  // { B_6, G_6, R_6, 0, B_7, G_7, R_7, 0 }
            __m256i pixel34 = _mm256_packs_epi32(pixel3, pixel4);// { BGR0_2, BGR0_3, BGR0_6, BGR0_7}

            __m256i RGBApels = _mm256_packus_epi16(pixel12, pixel34);

            _mm256_store_si256 (pRGBFirst256++, RGBApels);

            /* ---------- calc: upper row, high part (i.e. pels: 16 - 31 ) ---------- */
            R = OneColor256(Y32H, u32H, v32H, mat00_256, mat01_256, mat02_256, mat03_256);
            G = OneColor256(Y32H, u32H, v32H, mat10_256, mat11_256, mat12_256, mat13_256);
            B = OneColor256(Y32H, u32H, v32H, mat20_256, mat21_256, mat22_256, mat23_256);

            _mm_prefetch((char*)pRGBFirst256 ,_MM_HINT_T0 );

            tmp1    = GetShuffler<RGBShufflerType>::type::UnpackLowFirstRow(R, G, B, zero256);      // { B_0, R_0, B_1, R_1, B_4, R_4, B_5, R_5 }
            tmp2    = GetShuffler<RGBShufflerType>::type::UnpackLowFirstSecondRow(R, G, B, zero256); // { G_0, 0,   G_1, 0,   G_4, 0,   G_5, 0 }
            pixel1  = _mm256_unpacklo_epi32(tmp1,tmp2);  // { B_0, G_0, R_0, 0, B_4, G_4, R_4, 0 }
            pixel2  = _mm256_unpackhi_epi32(tmp1,tmp2);  // { B_1, G_1, R_1, 0, B_5, G_5, R_5, 0 }
            pixel12 = _mm256_packs_epi32(pixel1, pixel2);// { BGR0_0, BGR0_1, BGR0_4, BGR0_5}

            tmp1    = GetShuffler<RGBShufflerType>::type::UnpackHighFirstRow(R, G, B, zero256);       // { B_2, R_2, B_3, R_3, B_6, R_6, B_7, R_7 }
            tmp2    = GetShuffler<RGBShufflerType>::type::UnpackHighFirstSecondRow(R, G, B, zero256); // { G_2, 0,   G_3, 0,   G_6, 0,   G_7, 0 }
            pixel3  = _mm256_unpacklo_epi32(tmp1,tmp2);  // { B_2, G_2, R_2, 0, B_3, G_3, R_3, 0 }
            pixel4  = _mm256_unpackhi_epi32(tmp1,tmp2);  // { B_6, G_6, R_6, 0, B_7, G_7, R_7, 0 }
            pixel34 = _mm256_packs_epi32(pixel3, pixel4);// { BGR0_2, BGR0_3, BGR0_6, BGR0_7}

            RGBApels = _mm256_packus_epi16(pixel12, pixel34);

            _mm256_store_si256 (pRGBFirst256++, RGBApels);

            /* ---------- calc: lower row, low part (i.e. pels: 0 - 15 ) ---------- */

            Y    = _mm256_andnot_si256(maskF256, lower); // mask with 0xfff
            Y32  = _mm256_srli_epi16(Y, 4);    // { Y0_0, Y1_0, Y2_0, Y3_0, Y4_0, Y5_0, Y6_0, Y7_0, Y8_0, Y9_0, Y10_0, Y1_0, Y12_0, Y13_0, Y14_0, Y15_0 }
            Y32  = _mm256_permute4x64_epi64(Y32, _MM_SHUFFLE(3,1,2,0)); //{ Y0_0, Y1_0, Y2_0, Y3_0, Y8_0, Y9_0, Y10_0, Y11_0, Y4_0, Y5_0, Y6_0, Y7_0, Y12_0, Y13_0, Y14_0, Y15_0 }
            Y32L = _mm256_unpacklo_epi16(Y32, zero256);// { Y0_0, Y1_0, Y2_0, Y3_0, Y4_0, Y5_0, Y6_0, Y7_0 }
            Y32H = _mm256_unpackhi_epi16(Y32, zero256);// { Y8_0, Y9_0, Y10_0, Y11_0, Y12_0, Y13_0, Y14_0, Y15_0 }

            R = OneColor256(Y32L, u32L, v32L, mat00_256, mat01_256, mat02_256, mat03_256);
            G = OneColor256(Y32L, u32L, v32L, mat10_256, mat11_256, mat12_256, mat13_256);
            B = OneColor256(Y32L, u32L, v32L, mat20_256, mat21_256, mat22_256, mat23_256);

            _mm_prefetch((char*)pRGBSecond256 ,_MM_HINT_T0 );

            tmp1    = GetShuffler<RGBShufflerType>::type::UnpackLowFirstRow(R, G, B, zero256);       // { B_0, R_0, B_1, R_1, B_4, R_4, B_5, R_5 }
            tmp2    = GetShuffler<RGBShufflerType>::type::UnpackLowFirstSecondRow(R, G, B, zero256); // { G_0, 0,   G_1, 0,   G_4, 0,   G_5, 0 }
            pixel1  = _mm256_unpacklo_epi32(tmp1,tmp2);  // { B_0, G_0, R_0, 0, B_4, G_4, R_4, 0 }
            pixel2  = _mm256_unpackhi_epi32(tmp1,tmp2);  // { B_1, G_1, R_1, 0, B_5, G_5, R_5, 0 }
            pixel12 = _mm256_packs_epi32(pixel1, pixel2);// { BGR0_0, BGR0_1, BGR0_4, BGR0_5}

            tmp1    = GetShuffler<RGBShufflerType>::type::UnpackHighFirstRow(R, G, B, zero256);       // { B_2, R_2, B_3, R_3, B_6, R_6, B_7, R_7 }
            tmp2    = GetShuffler<RGBShufflerType>::type::UnpackHighFirstSecondRow(R, G, B, zero256); // { G_2, 0,   G_3, 0,   G_6, 0,   G_7, 0 }
            pixel3  = _mm256_unpacklo_epi32(tmp1,tmp2);  // { B_2, G_2, R_2, 0, B_3, G_3, R_3, 0 }
            pixel4  = _mm256_unpackhi_epi32(tmp1,tmp2);  // { B_6, G_6, R_6, 0, B_7, G_7, R_7, 0 }
            pixel34 = _mm256_packs_epi32(pixel3, pixel4);// { BGR0_2, BGR0_3, BGR0_6, BGR0_7}

            RGBApels = _mm256_packus_epi16(pixel12, pixel34);

            _mm256_store_si256 (pRGBSecond256++, RGBApels);

            /* ---------- calc: upper row, low part (i.e. pels: 16 - 31 ) ---------- */
            R = OneColor256(Y32H, u32H, v32H, mat00_256, mat01_256, mat02_256, mat03_256);
            G = OneColor256(Y32H, u32H, v32H, mat10_256, mat11_256, mat12_256, mat13_256);
            B = OneColor256(Y32H, u32H, v32H, mat20_256, mat21_256, mat22_256, mat23_256);

            _mm_prefetch((char*)pRGBSecond256 ,_MM_HINT_T0 );

            tmp1    = GetShuffler<RGBShufflerType>::type::UnpackLowFirstRow(R, G, B, zero256);       // { B_0, R_0, B_1, R_1, B_4, R_4, B_5, R_5 }
            tmp2    = GetShuffler<RGBShufflerType>::type::UnpackLowFirstSecondRow(R, G, B, zero256); // { G_0, 0,   G_1, 0,   G_4, 0,   G_5, 0 }
            pixel1  = _mm256_unpacklo_epi32(tmp1,tmp2);  // { B_0, G_0, R_0, 0, B_4, G_4, R_4, 0 }
            pixel2  = _mm256_unpackhi_epi32(tmp1,tmp2);  // { B_1, G_1, R_1, 0, B_5, G_5, R_5, 0 }
            pixel12 = _mm256_packs_epi32(pixel1, pixel2);// { BGR0_0, BGR0_1, BGR0_4, BGR0_5}

            tmp1    = GetShuffler<RGBShufflerType>::type::UnpackHighFirstRow(R, G, B, zero256);       // { B_2, R_2, B_3, R_3, B_6, R_6, B_7, R_7 }
            tmp2    = GetShuffler<RGBShufflerType>::type::UnpackHighFirstSecondRow(R, G, B, zero256); // { G_2, 0,   G_3, 0,   G_6, 0,   G_7, 0 }
            pixel3  = _mm256_unpacklo_epi32(tmp1,tmp2);  // { B_2, G_2, R_2, 0, B_3, G_3, R_3, 0 }
            pixel4  = _mm256_unpackhi_epi32(tmp1,tmp2);  // { B_6, G_6, R_6, 0, B_7, G_7, R_7, 0 }
            pixel34 = _mm256_packs_epi32(pixel3, pixel4);// { BGR0_2, BGR0_3, BGR0_6, BGR0_7}

            RGBApels = _mm256_packus_epi16(pixel12, pixel34);

            _mm256_store_si256 (pRGBSecond256++, RGBApels);
        }

        pRGBFirst = (unsigned char*)pRGBFirst256;
        pRGBSecond = (unsigned char*)pRGBSecond256;
        dataArrayFirst = (unsigned short*)dataArrayFirst256;
        dataArraySecond = (unsigned short*)dataArraySecond256;

    }


    template <>
    class GetShuffler<CRgbaShuffler> { public: typedef CRgbaShufflerAvx type; };

    template <>
    class GetShuffler<CBgraShuffler> { public: typedef CBgraShufflerAvx type; };

    template <>
    class GetShuffler<CArgbShuffler> { public: typedef CArgbShufflerAvx type; };

    template <>
    class GetShuffler<CAbgrShuffler> { public: typedef CAbgrShufflerAvx type; };
#endif
}

#endif //__ALGOAVX__


