#ifndef __ALGOD2D__
#define __ALGOD2D__

#include "OSUtilities.h"
#include "CommonAlgo.h"
#include "TiffFile.h"

namespace  InuCommon
{
    ////////////////////////////////////////////////////////////////////////
    /// \brief    Utility class for basic algorithms
    ///
    /// Responsibilities:
    ///      1. Knows how to convert disparity image to depth map.
    ////////////////////////////////////////////////////////////////////////
    class COMMONALGO_API CAlgoD2D
    {
    public:

        inline static int CalcDepth(
            int             disparity,
            float           disparityDivider,
            float           disparityOffset,
            float           opticalFactor,
            unsigned short  maxDisparity,
            uint32_t        disparityOffsetValue,
            int             nonValidDepthValue,
            float           iDepthScale
        )
        {
            if (disparity >= maxDisparity)
            {
                return nonValidDepthValue;
            }

            float depthF = CalcDepthF(
                disparity,
                disparityDivider,
                disparityOffset,
                opticalFactor,
                disparityOffsetValue,
                nonValidDepthValue,
                iDepthScale
            );

            //int depth = int(depthF + 0.5f);
            int depth = static_cast<int>(depthF);

            return depth;
        }

        inline static float CalcDepthF(
            int             disparity,
            float           disparityDivider,
            float           disparityOffset,
            float           opticalFactor,
            uint32_t        disparityOffsetValue,
            int             nonValidDepthValue,
            float           iDepthScale
        )
        {
            float depthF = opticalFactor / (((float)disparity + disparityOffsetValue) / disparityDivider + disparityOffset) * iDepthScale;

            return depthF;
        }

        /// \brief    Convert input disparity image into depth map.
        /// \param[out] outputBuffer    Output depth map (Z-Buffer).
        /// \param[in] inputBuffer    Input disparity image.
        /// \param[in] inputBufferSize    Input disparity buffer size.
        /// \param[in] opticalData    Optical data which is used for calculation.
        /// \param[in] confidenceThreshold    Minimum confidence.
        /// \param[in] maxdepth    Maximum depth value in output buffer.
        /// \param[in] nonValidDepthValue    Non valid depth value.
        /// \param[in] iHWModel    HW model (A0 or B0).
        /// \param[in] iRequestedLevel    Overwrites optimization level that is supported by the host machine.
        /// \param[in] iDisparityShiftRight    Only for NU4000.
        /// \param[in] iDisparityMask    Only for NU4000.
        /// \return  (optional) number of non valid depth pixels
        static uint32_t Disparity2Depth(
                unsigned short*             outputBuffer,
                const unsigned short*       inputBuffer,
                unsigned int                inputBufferSize,
                const CDepthOpticalData&    opticalData,
                unsigned int                confidenceThreshold,
                unsigned short              maxdepth,
                unsigned short              nonValidDepthValue,
                CTiffFile::EHWType          iHWModel,
                COsUtilities::EOptimizationLevel iRequestedLevel,
                float                       iScale,
                unsigned int                iDisparityShiftRight = 0,
                unsigned int                iDisparityMask = 0xFFFF,
                unsigned int                iConfidenceShiftRight = 0,
                unsigned int                iConfidenceMask = 0,
                uint32_t                    disparityFrameOffsetValue = 0,
                bool                        iWithConfidence=false
            );

        static uint32_t Disparity2Depth(
                unsigned short*             outputBuffer,
                const unsigned short*       inputBuffer,
                const std::shared_ptr<uint16_t>  iConfidenceMap,
                const std::shared_ptr<uint16_t> iDistanceMap,
                unsigned int                inputBufferSize,
                const CDepthOpticalData&    opticalData,
                unsigned short              nonValidDepthValue,
                CTiffFile::EHWType          iHWModel,
                COsUtilities::EOptimizationLevel iRequestedLevel,
                float                       iScale,
                unsigned int                iDisparityShiftRight = 0,
                unsigned int                iDisparityMask = 0xFFFF,
                unsigned int                iConfidenceShiftRight = 0,
                unsigned int                iConfidenceMask = 0,
                uint32_t                    disparityFrameOffsetValue = 0,
                bool                        iWithConfidence = false
            );

        static uint32_t Disparity2Depth(
                unsigned short*             outputBuffer,
                const unsigned short*       inputBuffer,
                const std::shared_ptr<uint16_t>  iConfidenceAndDisparityMap,
                unsigned int                inputBufferSize,
                const CDepthOpticalData&    opticalData,
                unsigned short              nonValidDepthValue,
                CTiffFile::EHWType          iHWModel,
                COsUtilities::EOptimizationLevel iRequestedLevel,
                float                       iScale,
                unsigned int                iDisparityShiftRight = 0,
                unsigned int                iDisparityMask = 0xFFFF,
                unsigned int                iConfidenceShiftRight = 0,
                unsigned int                iConfidenceMask = 0,
                uint32_t                    disparityFrameOffsetValue = 0,
                bool                        iWithConfidence = false
            );

		static uint32_t Depth2DistanceDepth(
			uint16_t*						outBuffer,
			const uint16_t*					inBuffer,
			const std::shared_ptr<uint16_t>	iDistanceMap,
			uint32_t						iBufferSize,
			uint16_t						iNonValidDepthValue
		);

        static bool InitDistanceMap(
                std::shared_ptr<uint16_t>   iDistanceMap,
                uint32_t                    iHeight,
                uint32_t                    iWidth,
                uint32_t                    iMaxDistance,
                float                       iScale,
                bool                        iWithconfidence,
                float                       iFocalLengthX,
                float                       iFocalLengthY
            );

        static bool InitConfidenceMap(
                std::shared_ptr<uint16_t>  iConfidenceMap,
                uint32_t                    iHeight,
                uint32_t                    iWidth,
                uint32_t                    iConfidneceStartRegion2,
                uint32_t                    iConfidneceEndRegion2,
                uint16_t                    iConfidenceThresholdRegion1,
                uint16_t                    iConfidenceThresholdRegion2
            );

        static bool InitConfidenceAndDisparityMap(
                std::shared_ptr<uint16_t>    iConfidenceAndDistanceMap,
                unsigned int                 iHeight,
                unsigned int                 iWidth,
                unsigned int                 iConfidneceStartRegion2,
                unsigned int                 iConfidneceEndRegion2,
                uint16_t                     iConfidenceThresholdRegion1,
                uint16_t                     iConfidenceThresholdRegion2,

                uint32_t                    iMaxDistance,
                float                       iScale,
                bool                        iWithconfidence,
                float                       iFocalLengthX,
                float                       iFocalLengthY,

                uint16_t                    confidenceMask,
                unsigned short              confidenceShift,
                uint16_t                    disparityMask,
                unsigned short              disparityShift,

                float                       opticalFactor,
                float                       disparityOffset,
                float                       disparityDivider,
                uint32_t                    disparityFrameOffsetValue
            );

    };
}

#endif
