#ifndef __ALGOBASE__
#define __ALGOBASE__

#include "OSUtilities.h"
#include "CommonAlgo.h"
#include "TiffFile.h"
#include "AlgoDepthOpticalData.h"

#include <memory>

namespace cv
{
    class Mat;
}

namespace  InuCommon
{
    ////////////////////////////////////////////////////////////////////////
    /// \brief    Utility class for basic algorithms
    ///
    /// Responsibilities:
    ///      1. Knows how to convert disparity image to depth map.
    ////////////////////////////////////////////////////////////////////////
    class COMMONALGO_API CAlgoBase
    {
    public:

        // These 3 LUTS are used by the algorithm
        static const int            LUT_SIZE = 256;
        static const unsigned char  RLut[LUT_SIZE];
        static const unsigned char  GLut[LUT_SIZE];
        static const unsigned char  BLut[LUT_SIZE];


        static int Depth2XYZ(
            const unsigned short * inDepthMap,
            unsigned int width,
            unsigned int height,
            float f_x,
            float f_y,
            int opt_cen_x,
            int opt_cen_y,
            unsigned int minZ,
            unsigned int maxZ,
            float* outXYZMat,
            float iScale,
            int iPointCloudZAxis,
            int iPointCloudYAxis,
            std::pair<unsigned int, unsigned int> iDecimationFactor,
            bool iWithConfidence = false,
            COsUtilities::EOptimizationLevel  iRequestedLevel = COsUtilities::EOptimizationLevel::eNone);

        static int Depth2XYZ_SSE(
            const unsigned short * inDepthMap,
            unsigned int width,
            unsigned int height,
            float f_x,
            float f_y,
            int opt_cen_x,
            int opt_cen_y,
            float decimation,
            unsigned int minZ,
            unsigned int maxZ,
            float* outXYZMat,
            float iScale,
            int iPointCloudZAxis,
            int iPointCloudYAxis,
            bool iWithConfidence = false,
            COsUtilities::EOptimizationLevel  iRequestedLevel = COsUtilities::EOptimizationLevel::eNone
        );

        static void Depth2XYZValid(
                const cv::Mat& inDepthMap,
                float f_x ,
                float f_y,
                int opt_cen_x,
                int opt_cen_y,
                float xy_binning,
                int minZ,
                int maxZ,
                cv::Mat &outXYZMat
            );

        /// \brief    Disparity Improvement Algorithm (fast mode with video)
        /// \param[in] ioDisparity    Input disparity image that is improved.
        /// \param[in] iYimage    Input Y image.
        /// \param[in,out] iImageSize    Image size (width * height).
        /// \param[in,out] iRequestedLevel    Optimization level.
        static void DisparityImrovementFastMode(
                unsigned short* ioDisparity,
                const unsigned short* iYimage,
                unsigned int iWidth,
                unsigned int iHeight,
                unsigned int iAlg,
                unsigned int iThreshold,
                unsigned int iMinDisparity,
                unsigned int iMaxDisparity,
                unsigned int iDecimation,
                std::shared_ptr<bool> ioWorkingBuffer,
                std::shared_ptr<unsigned short> ioFillHolesBuffer,
                COsUtilities::EOptimizationLevel iRequestedLevel
            );

#ifndef NO_BASIC_ALGO

        /// \brief    Generate Register3D Image
        /// \param[in] matXYZ       XYZ Image.
        /// \param[in] matRGBRef    RGB Image.
        /// \param[in] oBuff        output buff.
        /// \param[in] oBuff        output buff.
        /// \param[in] iConfidence  Point cloud was created with confidence.
        /// \param[in] iDecimationFactor The decimation factor (n) is used to pick only every n-th pixel in both axes horizontal and vertical.
        /// \return uint32_t        number of points
        static uint32_t Register3D(const cv::Mat& matXYZ, const cv::Mat& matRGBRef, std::shared_ptr<uint8_t> oBuff, uint32_t buffSize, bool iConfidence, const std::pair<unsigned int, unsigned int>& iDecimationFactor = std::make_pair(1,1));


        static bool WriteToPly(const cv::Mat& matXYZ, const cv::Mat& matRGBRef,const std::string& outName, bool binary_only);

        /// \brief    Generate PLY file (Grayscale).
        /// \param[in] matXYZ    Depth Image.
        /// \param[in] size   Number of vertex.
        /// \param[in] outName    Created file name.
        /// \param[in] iConfidence  Point cloud was created with confidence.
        /// \return bool    description.
        static bool WriteToPly(const float* pXYZ, int size, const std::string& outName, bool iConfidence);
#endif

#ifndef NO_ALGO
        static void Disparity2XYZ(const cv::Mat& inDispMap, float f, float B, float d_offset, int opt_cen_x, int opt_cen_y, float xy_binning, int minZ, int maxZ, cv::Mat &outXYZMat, cv::Mat &Zmat);
        static void Disparity2XYZ_B0(const cv::Mat& inDispMap, float f, float B, float d_offset, int opt_cen_x, int opt_cen_y, float xy_binning, int minZ, int maxZ, cv::Mat &outXYZMat, cv::Mat &Zmat);
        static bool ParseiregData(const std::string& iregfile, unsigned short &dispoffset);
#endif

        static void SetNumOfDepthConfidenenceBits(unsigned short iInput) { sNumOfDepthConfidenenceBits = iInput; }

        static unsigned short sNumOfDepthConfidenenceBits;

    private :

        /// \brief  Bit mask
        enum EFastModeAlg
        {
            eOnlyY = 0,
            eMaskY = 1,
            eFillHoles = 2
        };

        static bool WritePLYHeader(const std::string& fileName, int nVertex, bool depthOnly, bool binary);

        /// \brief  Executes DisparityImrovementFastMode on one line
        static void DisparityImrovementFastModeOneLine(
                unsigned short* iDisparity,
                unsigned short *disparityLineBuffer,
                const unsigned short* iYimage,
                unsigned int iWidth,
                unsigned short iDMin,
                unsigned short iDMax,
                unsigned int iThreshold
            );

        /// \brief Calculate disparity map histogram
        static bool DisparityImrovementFastHistogram(
                unsigned short *histogram,
                unsigned short* iDisparity,
                unsigned int iWidth,
                unsigned int iHeight,
                unsigned int &iDMin,
                unsigned int &iDMax
            );
    };
}

#endif
