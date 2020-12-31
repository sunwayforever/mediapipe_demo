#pragma once

#include <functional>

namespace InuCommon
{		
	/// \brief    Pixel Serializer - write to file
	///
	/// \param[in] iBuffer      data buffer to write
	/// \param[out] oString      output string that is flushed to file 
	typedef std::function<int(const unsigned char*, char*)> PixelSerializer;

	/// \brief    Pixel Serializer - read from file
	///
	/// \param[in] iLine        data line that is read from file
	/// \param[in] oBuffer      output data buffer
	typedef std::function<int(const std::string &, unsigned char*)> PixelDeserializer;
}
