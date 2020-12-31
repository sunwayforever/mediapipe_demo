/*
* File - ImuStream.h
*
* This file is part of the Inuitive SDK
*
* Copyright (C) 2014 All rights reserved to Inuitive
*
*/

#ifndef __CNNSTREAMH__
#define __CNNSTREAMH__

#include "GeneralFrame.h"
#include "BaseOutputStream.h"
#include "InuSensor.h"

#include <functional>
#include <string>

namespace InuDev
{
    ////////////////////////////////////////////////////////////////////////
    /// \brief    Represents a CNN frame
    ///
    /// Comment: The CNN frame conssist of the data which is part of the CGeneralFrame & the
    ///          CCnnFrameHdr which is the meta data which describes the data.
    ////////////////////////////////////////////////////////////////////////
    struct CCnnFrame : public CGeneralFrame
    {
        static const int CNN_FRAME_MAX_TAILS = 30;
        ////////////////////////////////////////////////////////////////////////
        /// \brief CCnnTailHdr: This structure describes the output buffer of a network
        ////////////////////////////////////////////////////////////////////////
        struct CCnnTailHdr
        {
            int32_t ElementSize;    // Element size in bytes
            int32_t ElementCount;   // Number of elements in a tail
            int32_t Width;          // Buffer width in element
            int32_t Height;         // Buffer height in element
            int32_t Inputs;         // Number of input maps

            union {
                // Fields relevant for ceva
                struct {
                    int32_t FracBits;       // In case of using fixed point: the scale factor in which the fixed point data is represented.
                                            // Output x(float) is represented as x(float) = scale_factor * x(fixed)
                } ceva_fields;
                // Fields relevant for Synopsys
                struct {
                    float scale;			// Fixed point values shoule be divided by this scale
                    int alloc_count;		// Relevant for Bboxes output - Number of allocated bboxes
                    int valid_count;		// Relevant for Bboxes output - Number of valid bboxes
                    int bbox_scale;			// Relevant for Bboxes output - Use short with scale instead of float
                    int confidence_scale;	// Relevant for Bboxes output - Use short with scale instead of float
                } synopsys_fields;
            }CnnFields;
        };

        //////////////////////////////////////////////////////////////////////////////////
        /// \brief CCnnFrameHdr: is the description of the result buffer from dsp to
        //////////////////////////////////////////////////////////////////////////////////
        struct CCnnFrameHdr
        {
            uint32_t TotalResultSize;   // Total results size
            uint32_t NumOfTails;        // Number of concatenated CCnnTailHdr, at the results blob
            int32_t NetworkId;          // The network id that had returned the result
            int32_t FrameId;            // Webcam frame number
            CCnnTailHdr TailHeader[CNN_FRAME_MAX_TAILS]; // tail header for each tail
            uint32_t OffsetToBlob[CNN_FRAME_MAX_TAILS];  // offset to tail , assuming tail 0 offset is 0
        };

        ///  \brief     Frame header
        CCnnFrameHdr FrameHeader;

        ///  \brief     Engine Type: ceva or synopsis
        CCnnLoadParams::ECnnNetworkEngineType EngineType;

        /// \brief    Constructs empty (zero size) buffer
        CCnnFrame() : CGeneralFrame() { }

        /// \brief    Constructor.
        /// \param[in] bufferSize    generated buffer size.
        CCnnFrame(uint32_t iBufferSize) : CGeneralFrame(iBufferSize) {}

        /// \brief    Destructor.
        virtual ~CCnnFrame() {}
    };

    typedef enum
    {
        FW_NUCFG_FORMAT_GREY_16_E = 0,
        FW_NUCFG_FORMAT_BAYER_16_E,
        FW_NUCFG_FORMAT_RGB888_E,
        FW_NUCFG_FORMAT_RGB666_E,
        FW_NUCFG_FORMAT_RGB565_E,
        FW_NUCFG_FORMAT_RGB555_E,
        FW_NUCFG_FORMAT_RGB444_E,
        FW_NUCFG_FORMAT_YUV420_8BIT_E,
        FW_NUCFG_FORMAT_YUV420_8BIT_LEGACY_E,
        FW_NUCFG_FORMAT_YUV420_10BIT_E,
        FW_NUCFG_FORMAT_YUV422_8BIT_E,
        FW_NUCFG_FORMAT_YUV422_10BIT_E,
        FW_NUCFG_FORMAT_RAW6_E,
        FW_NUCFG_FORMAT_RAW7_E,
        FW_NUCFG_FORMAT_RAW8_E,
        FW_NUCFG_FORMAT_RAW10_E,
        FW_NUCFG_FORMAT_RAW12_E,
        FW_NUCFG_FORMAT_RAW14_E,
        FW_NUCFG_FORMAT_GEN_8_E,
        FW_NUCFG_FORMAT_GEN_12_E,
        FW_NUCFG_FORMAT_GEN_16_E,
        FW_NUCFG_FORMAT_GEN_24_E,
        FW_NUCFG_FORMAT_GEN_32_E,
        FW_NUCFG_FORMAT_GEN_64_E,
        FW_NUCFG_FORMAT_GEN_96_E,
        FW_NUCFG_FORMAT_GEN_672_E,
        FW_NUCFG_FORMAT_DEPTH_E,
        FW_NUCFG_FORMAT_DISPARITY_E,
        FW_NUCFG_FORMAT_DISPARITY_DEBUG_E,
        FW_NUCFG_FORMAT_HISTOGRAM_E,
        FW_NUCFG_FORMAT_NA_E,
        FW_NUCFG_FORMAT_NUM_FORMATS_E,
    } FW_NUCFG_formatE;

    typedef enum
    {
        FW_NUCFG_INTER_MODE_LINE_BY_LINE_E = 0,
        FW_NUCFG_INTER_MODE_PIXEL_BY_PIXEL_E = 1,
        FW_NUCFG_INTER_MODE_FRAME_BY_FRAME_E = 2,
        FW_NUCFG_INTER_MODE_UNKNOWN_E = 3,
        FW_NUCFG_NUM_OF_INT_MODE_E
    } FW_NUCFG_interModeE;

    ////////////////////////////////////////////////////////////////////////
    /// \brief    Image Descriptor
    ///
    /// Role: Describes the input buffer for a CNN network.
    ///
    ////////////////////////////////////////////////////////////////////////
    struct CCnnImageDescriptor
    {
        uint32_t width;             //image width
        uint32_t height;            //image height
        FW_NUCFG_formatE format;       //RGB YUV RAW
        uint32_t x;                 //ROI x starting point
        uint32_t y;                 //ROI y starting point
        uint32_t stride;            //Width of image buffer in memory (the step needed to jump to next line)
        uint32_t bufferHeight;          //Image buffer height
        uint32_t bitsPerPixel;          //number of bits per pixel
        uint32_t realBitsMask;          //repacker related
        uint32_t numInterleaveImages;   //number of images in buffer
        FW_NUCFG_interModeE interMode;     //interleave mode: line by line, pixel by pixel, frame by frame

        CCnnImageDescriptor() :
            width(0),
            height(0),
            format(FW_NUCFG_FORMAT_GREY_16_E),
            x(0),
            y(0),
            stride(0),
            bufferHeight(0),
            bitsPerPixel(0),
            realBitsMask(0),
            numInterleaveImages(0),
            interMode(FW_NUCFG_INTER_MODE_LINE_BY_LINE_E) {}
    };

    ////////////////////////////////////////////////////////////////////////
    /// \brief    General CNN Stream
    ///
    /// Role: Load/run and analyze a CNN on device.
    ///
    /// Responsibilities:
    ///      1. control the service (Init, Terminate, Start and Stop).
    ///      2. control CNN on the device (Start, Stop)
    ///      3. provide a continuous stream of result frames
    ///
    ////////////////////////////////////////////////////////////////////////
    class CCnnStream : public CBaseOutputStream
    {
    public:

        typedef std::function<void(std::shared_ptr<CCnnStream>, std::shared_ptr<const CCnnFrame>, CInuError)> CallbackFunction;

        virtual ~CCnnStream() {}

        /// \brief    Retrieves new CNN image frame (pull)
        ///
        /// This method returns when a new frame is ready (blocking) or if an input timeout has elapsed.
        /// It shall be called only after a Start() was is invoked but before any invocation of a Stop() is invoked.
        /// \param[out] oCNNFrame    The returned CNN frame (char buffer) that can be analyzed by the caller application.
        /// \param[in] iTimeout    Function is returned if timeout has elapsed even if no new frame is ready.
        /// \return CInuError    Error code, InDev::eOK if operation successfully completed.
        virtual CInuError GetFrame(std::shared_ptr<const CCnnFrame>& oCNNFrame, unsigned int iTimeout = FPS_BASED_TIMEOUT) = 0;

        /// \brief    Registration/Unregistration for receiving stream of CNN frames (push)
        ///
        /// The provided callback function is called when a new frame is ready (non-blocking).
        /// It shall be called only after a Start() was is invoked but before any invocation of a Stop() is invoked.
        /// \param[in] iCallback    Callback function which is invoked when a new frame is ready.
        ///                         Send nullptr to unregister for receiving frames.
        /// \param[in] iTimeout    Callback is invoked if a timeout has elapsed even if no new frame is ready.
        /// \return CInuError    Error code, InDev::eOK if operation successfully completed.
        virtual CInuError Register(CallbackFunction iCallback, unsigned int iTimeout = FPS_BASED_TIMEOUT) = 0;


        /// \brief    Start CNN identified by its unique ID & The node in the "sw graph" where the network should execute.
        /// \param[in] iNetworkID    The unique identifier of the network.
        /// \param[in] iNetworkNodeName    The node in the "sw graph" where the network should execute.
        /// \return InuDev::CInuError    Error code, InDev::eOK if operation successfully completed.
        virtual CInuError StartNetwork(const std::string& iNetworkID, const std::string& iNetworkNodeName) = 0;

        /// \brief    Stop a CNN network.
        /// \return InuDev::CInuError    Error code, InDev::eOK if operation successfully completed.
        virtual CInuError StopNetwork() = 0;

        /// \brief    Write buffer to CNN service.
        /// \param[in] iStreamer  The name of the input streamer in the "sw graph", the buffer will be written to.
        /// \param[in] iImageDescriptor    Describes the buffer.
        /// \param[in] iBuffer      Pointer to buffer.
        /// \param[in] iBufferLen   Buffer length.
        /// \return InuDev::CInuError    Error code, InDev::eOK if operation successfully completed.
        virtual CInuError WriteData(const std::string& iStreamer, const CCnnImageDescriptor& iImageDescriptor, const unsigned char* iBuffer, unsigned int iBufferLen, int32_t iFrameId=0) = 0;
    };
}

#endif // __CNNSTREAM_H__
