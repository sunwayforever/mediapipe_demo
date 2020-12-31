/*
* File - GeneralFrames.h
*
* This file is part of the Inuitive SDK
*
* Copyright (C) 2014 All rights reserved to Inuitive
*
*/

#ifndef __GENERALFRAME_H__
#define __GENERALFRAME_H__

#include "BaseFrame.h"

namespace InuDev
{

    ////////////////////////////////////////////////////////////////////////
    /// \brief    Represents generic buffer that is provided by some of InuDev streams
    ///
    /// Responsibilities:
    ///      1. Buffer attributes: data and size.
    ///      2. Memory management control.
    ///
    /// Comment: The interpretation of provided buffers should be done by the caller application.
    ///          The caller application should be familiar with the internal format of
    ///          each provided frame.
    ////////////////////////////////////////////////////////////////////////
    class  CGeneralFrame : public CBaseFrame
    {
        friend class CBaseGeneralFrameImp;
    public:

        /// \brief    Empty constructor.
        INUSTREAMS_API CGeneralFrame();

        /// \brief    Constructor.
        /// \param[in] bufferSize    generated buffer size.
        INUSTREAMS_API CGeneralFrame(uint32_t iBufferSize);

        /// \brief    Copy constructor - deep Copy.
        INUSTREAMS_API CGeneralFrame(const CGeneralFrame& input);

        /// \brief    Move constructor.
        INUSTREAMS_API CGeneralFrame(CGeneralFrame&& input);

        /// \brief    Assignment operator - deep copy.
        INUSTREAMS_API CGeneralFrame& operator=(const CGeneralFrame& input);

        /// \brief    Move assignment operator.
        INUSTREAMS_API CGeneralFrame& operator=(CGeneralFrame&& input);

        INUSTREAMS_API virtual ~CGeneralFrame();

        /// \brief    Non mutable pointer to image data
        INUSTREAMS_API InuDev::byte* const GetData() const { return mData; }

        /// \brief    Pointer to data buffer
        INUSTREAMS_API InuDev::byte* GetData() { return mData; }

        /// \brief    Buffer size
        INUSTREAMS_API uint32_t GetDataSize() const { return mDataSize;  }

        /// \brief    ExtractDataBuffer() - Extracts the data buffer from this object
        ///
        /// Returns the data buffer, and removes the ownership.
        /// This object won't hold the buffer anymore, and is no longer responsible for freeing the buffer.
        /// \return InuDev::byte*     data buffer pointer
        INUSTREAMS_API InuDev::byte*   ExtractDataBuffer();

		/// \brief    Overwrites the buffer size
		void SetBufferSize(uint32_t iBufferSize) { mDataSize = iBufferSize; }

    protected:

        /// \brief    Constructor that can be used only by derived classes
        /// \param[in] iBufferSize    generated buffer
        /// \param[in] iBuffer      generated buffer size.
        /// \param[in] iOwnData     true if this object should own the buffer or just hold it .
        CGeneralFrame(InuDev::byte* iBuffer, uint32_t iBufferSize, bool iOwnBuffer = true);

        /// \brief    Create empty buffer with the input size
        void Init(uint32_t iBufferSize);

    private:

        /// \brief  Data buffer
        InuDev::byte* mData;

        ///  \brief  Buffer size
        uint32_t mDataSize;

        /// \brief    If data was extracted (by using ExtractDataBuffer) then this flag becomes false and data will not be released.
        bool mReleaseData;
    };
}

#endif // __BASEFRAME_H__
