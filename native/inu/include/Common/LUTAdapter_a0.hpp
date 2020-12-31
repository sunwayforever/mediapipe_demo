#pragma once
#include <exception>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <vector>

#include "GlobalDefines_a0.h"
// includes needed for specific formats
#include "DataFormats_a0.h"
#include "OSUtilities.h"

static bool ReadIB32bitSingleChannel_internal(const std::string &filename,
                                              std::vector<uint32> &elements,
                                              InputFormat fmt);
static bool ReadIB32bit3Channels_internal(const std::string &filename,
                                          std::vector<uint32> &elements,
                                          InputFormat fmt);
static bool ReadDrectRawTable_internal(const std::string &filename,
                                       std::vector<uint32> &elements,
                                       InputFormat fmt);

// General adapter for LUTs read/write in different formats
// The adapter gets serialized elements (as std::vector) and writes to a file in
// the requested format , or vise-versa In this way, format changes affect only
// the code of LUTAdapter, not every importLUTs/exportLUTs func in blocks
class LUTAdapter_a0 {
   public:
    class InvalidArgumentException : public std::exception {
       public:
        InvalidArgumentException(const char *msg) : std::exception() {}
    };

    class InvalidFormatException : public std::exception {
       public:
        InvalidFormatException(const char *msg) : std::exception() {}
    };

    // typedef enum TableLayout { Address_Value, Only_Values };
    enum DataType { Hex, Bin };

    LUTAdapter_a0(const std::string &filename) : m_Filename(filename) {}

    virtual ~LUTAdapter_a0() {}

    // Arguments:
    // elements       : The elements vector (pay attention that the type should
    // match the template argument (e.g : LUTAdapter::Write<uint32> gets
    // vector<uint32> argument) data_type      : Hex, Bin, etc. (by the enum)
    // word_info_bits : how many bits should contain information
    // word_total_width_bytes : Total written length. If larger than width of
    // info bits, then padded with '0'
    template <class T>
    bool Write(const std::vector<T> &elements, DataType data_type = Hex,
               uint8 word_info_bits = 32, uint8 word_total_width_bytes = 0) {
        return Write(m_Filename, elements, data_type, word_info_bits);
    }

    template <class T>
    static bool Write(const std::string &filename,
                      const std::vector<T> &elements, DataType data_type = Hex,
                      uint8 word_info_bits = 32,
                      uint8 word_total_width_bytes = 0);

    // Arguments : Similar to write
    // elements       : The elements vector (pay attention that the type should
    // match the template argument (e.g : LUTAdapter::Read<uint32> gets
    // vector<uint32> argument)) data_type      : Hex, Bin, etc. (by the enum)
    // word_info_bits : How many bits should contain information out of the
    // whole word word_total_width_bytes : Length of word to read overwrite : If
    // true, does not change vector's size, assuming the size can hold all added
    // elements. If false - appends to vector
    template <class T>
    bool Read(std::vector<T> &elements, DataType data_type = Hex,
              uint8 word_info_bits = 32, uint8 word_total_width_bytes = 0) {
        return Read(m_Filename, elements, data_type, word_info_bits);
    }

    template <class T>
    static bool Read(const std::string &filename, std::vector<T> &elements,
                     DataType data_type = Hex, uint8 word_info_bits = 32,
                     uint8 word_total_width_bytes = 0);

    //// Format-specific readers/writers/converters

    // Writes binary stream of 32 bit in a split way (
    template <class T>
    static void BinWriter32Split(std::ofstream &ofs,
                                 const std::vector<T> &elements);

    template <class T>
    static void BinWriter32Doubled(std::ofstream &ofs,
                                   const std::vector<T> &elements);

    // Moves elements from src to dst - pack them in 32 bit
    template <class T>
    static void ConvertTo32bit(const std::vector<T> &src,
                               std::vector<uint32> &dst);

    // Read IB LUT of a single channel (gray) - 32 bit word containing 10 bit
    // data (LSB) Output (elements) is a vector containing IB data of 3 channels
    // (duplicated data)
    static bool ReadIB32bitSingleChannel(const std::string &filename,
                                         std::vector<uint32> &elements,
                                         InputFormat fmt) {
        // Split to 2 funcs because of a linkage problem
        return ReadIB32bitSingleChannel_internal(filename, elements, fmt);
    }

    // Read IB LUT of RGB (3 channels) - 32 bit word containing 30 bit (10x3)
    // data (LSB) Output (elements) is a vector containing IB data of 3 channels
    // (duplicated data)
    static bool ReadIB32bit3Channels(const std::string &filename,
                                     std::vector<uint32> &elements,
                                     InputFormat fmt) {
        // Split to 2 funcs because of a linkage problem
        return ReadIB32bit3Channels_internal(filename, elements, fmt);
    }

    // Read raw drect table (Matlab output) to an elements array
    static bool ReadDrectRawTable(const std::string &filename,
                                  std::vector<uint32> &elements,
                                  InputFormat fmt) {
        // Splitted to 2 funcs because of a linkage problem
        return ReadDrectRawTable_internal(filename, elements, fmt);
    }

   private:
    explicit LUTAdapter_a0(){};
    explicit LUTAdapter_a0(const LUTAdapter_a0 &){};

    static inline uint64 CreateMask64(uint8 width) {
        uint64 mask = 0;
        for (int i = 0; i < width; ++i) {
            mask <<= 1;
            mask |= 1;
        }
        return mask;
    }

    template <class T>
    static inline void WriteHeader(std::ofstream &ofs,
                                   const std::vector<T> &elements) {
        uint32 first_value = 0x557edd0a;  //=> 0x0add7e55; (address)
        uint32 second_value = (uint32)(elements.size() * 2 * sizeof(T));
        ofs.write((const char *)&first_value, sizeof(uint32));
        ofs.write((const char *)&second_value, sizeof(uint32));
    }

   private:
    std::string m_Filename;
};

///////////////////////////////// Writers
/////////////////////////////////////////////////

// Writes binary stream of 32 bit in a split way (old format - replaced
// on 10.9.13)
template <class T>
void LUTAdapter_a0::BinWriter32Split(std::ofstream &ofs,
                                     const std::vector<T> &elements) {
    const char *zero =
        "\0\0\0";  // this is actually 4x'\0' (written to file as '0000)
    int vec_size = (int)elements.size();
    for (int i = 0; i < vec_size; i += 2) {
        ofs.write((const char *)(&elements[i]), sizeof(T));
        ofs.write(zero, 4);
    }
    for (int i = 1; i < vec_size; i += 2) {
        ofs.write(zero, 4);
        ofs.write((const char *)(&elements[i]), sizeof(T));
    }
}

// Writes binary stream of 32 bit 2 times = 64bit
template <class T>
void LUTAdapter_a0::BinWriter32Doubled(std::ofstream &ofs,
                                       const std::vector<T> &elements) {
    // WriteHeader(ofs, elements);
    for (int i = 0; i < (int)elements.size(); ++i) {
        ofs.write((const char *)(&elements[i]), sizeof(T));
        ofs.write((const char *)(&elements[i]), sizeof(T));
    }
}

// Moves elements from src to dst - pack them in 32 bit
template <class T>
void LUTAdapter_a0::ConvertTo32bit(const std::vector<T> &src,
                                   std::vector<uint32> &dst) {
    int bit_size = sizeof(T) * 8;
    int factor = 32 / bit_size;
    uint32 temp32;
    if (factor < 1) throw std::exception();
    typename std::vector<T>::const_iterator iter = src.begin();
    while (iter < src.end()) {
        temp32 = 0;
        for (int i = 0; i < factor && iter < src.end(); ++i) {
            temp32 |= (*(iter++) << i * bit_size);
        }
        dst.push_back(temp32);
    }
}

template <class T>
bool LUTAdapter_a0::Write(const std::string &filename,
                          const std::vector<T> &elements, DataType data_type,
                          uint8 word_info_bits, uint8 word_total_width_bytes) {
    bool result = true;
    uint64 temp64;
    typename std::vector<T>::const_iterator iter = elements.begin();
    const uint8 elem_size_bits = sizeof(T) * 8;
    int elems_per_word = word_info_bits / (sizeof(T) * 8);
    std::ofstream ofs;

    // Arguments validation
    if (word_info_bits != 32 && word_info_bits != 64)
        throw InvalidArgumentException("Invalid word size");

    // Open file
    if (data_type == Hex)
        ofs.open(filename.c_str(), std::ofstream::out);
    else
        ofs.open(filename.c_str(), std::ofstream::out | std::ofstream::binary);

    if (!ofs.good()) return false;

    if (data_type == Hex) {
        // Setting word format
        ofs << std::hex;

        while (iter < elements.end()) {
            if (word_total_width_bytes > 0) {
                // Set padding
                ofs << std::setw(word_total_width_bytes);
                ofs << std::setfill('0');
            }

            temp64 = 0;
            for (int k = 0; k < elems_per_word && iter < elements.end();
                 ++k, ++iter) {
                // temp64 <<= elem_size_bits;
                // temp64 |= (*iter & mask);
                temp64 |= (uint64(*iter) << (elem_size_bits * k));
            }

            ofs << temp64;

            if (data_type == Hex) ofs << std::endl;
        }
    } else {
        ofs << std::dec;
        // Writing 32 bit doubled to 64bit (ABCDEF -> AABBCCDD):
        if (sizeof(T) == 32)
            BinWriter32Doubled(ofs, elements);
        else {
            std::vector<uint32> binvec32;
            ConvertTo32bit(elements, binvec32);
            BinWriter32Doubled(ofs, binvec32);
        }
    }

    ofs.close();

    return result;
}

///////////////////////////////// Readers
/////////////////////////////////////////////////

inline bool ReadIB32bitSingleChannel_internal(const std::string &filename,
                                              std::vector<uint32> &elements,
                                              InputFormat fmt) {
    // This code is copied from the IntensityBalance block,and hasn't been
    // changed to fit the rest of the code here (i.e using streams, template,
    // etc.)

    int lut_size_x;
    int lut_size_y;

    if (fmt == VGA) {
        lut_size_x = LUT_SIZE_BINNED_X;
        lut_size_y = LUT_SIZE_BINNED_Y;
    } else {
        lut_size_x = LUT_SIZE_FR_X;
        lut_size_y = LUT_SIZE_FR_Y;
    }

    int grid_size_x = MAX_IB_LUT_LINE_SIZE;

    FILE *file;
    InuCommon::COsUtilities::fopen(file, filename.c_str(), "rb");

    if (file == NULL) return false;

    uint32 *intdata = new uint32[lut_size_x * lut_size_y];
    fread(intdata, sizeof(uint32), lut_size_x * lut_size_y, file);
    fclose(file);

    for (int yb = 0; yb < lut_size_y; ++yb) {
        for (int xb = 0; xb < lut_size_x; ++xb) {
            int k = xb + lut_size_x * yb;

            // Validation that data size is up to 10 bits
            if (intdata[k] != (intdata[k] & 0x3ff))
                throw LUTAdapter_a0::InvalidFormatException(
                    "Expected only 10 bits of information, found more");

            elements.push_back(intdata[k] |
                               (intdata[k] << IB_LUT_CHANNEL_BITS) |
                               (intdata[k] << (IB_LUT_CHANNEL_BITS * 2)));
        }

        // Pad to end of line
        for (int i = lut_size_x; i < grid_size_x; ++i) elements.push_back(0);
    }

    // Pad line to end of file
    for (int i = (int)elements.size(); i < MAX_IB_LUT_SIZE; ++i)
        elements.push_back(0);

    delete[] intdata;

    return true;
}

inline bool ReadIB32bit3Channels_internal(const std::string &filename,
                                          std::vector<uint32> &elements,
                                          InputFormat fmt) {
    // This code is copied from the IntensityBalance block,and hasn't been
    // changed to fit the rest of the code here (i.e using streams, template,
    // etc.)

    int lut_size_x;
    int lut_size_y;

    if (fmt == VGA) {
        lut_size_x = LUT_SIZE_BINNED_X;
        lut_size_y = LUT_SIZE_BINNED_Y;
    } else {
        lut_size_x = LUT_SIZE_FR_X;
        lut_size_y = LUT_SIZE_FR_Y;
    }

    int grid_size_x = MAX_IB_LUT_LINE_SIZE;

    FILE *file;
    InuCommon::COsUtilities::fopen(file, filename.c_str(), "rb");

    if (file == NULL) return false;

    uint32 *intdata = new uint32[lut_size_x * lut_size_y];
    size_t readData =
        fread(intdata, sizeof(uint32), lut_size_x * lut_size_y, file);
    fclose(file);

    if (readData < size_t(lut_size_x * lut_size_y)) return false;

    for (int yb = 0; yb < lut_size_y; ++yb) {
        for (int xb = 0; xb < lut_size_x; ++xb) {
            int k = xb + lut_size_x * yb;

            // Validation that data is up to 30 bits
            if (intdata[k] != (intdata[k] & 0x3fffffff))
                throw LUTAdapter_a0::InvalidFormatException(
                    "Expected only 30 bits of information, found more");

            elements.push_back(intdata[k]);
        }

        // Pad to end of line
        for (int i = lut_size_x; i < grid_size_x; ++i) elements.push_back(0);
    }

    // Pad line to end of file
    for (int i = (int)elements.size(); i < MAX_IB_LUT_SIZE; ++i)
        elements.push_back(0);

    delete[] intdata;

    return true;
}

inline bool ReadDrectRawTable_internal(const std::string &filename,
                                       std::vector<uint32> &elements,
                                       InputFormat fmt) {
    int image_size_x = XDIM_MAX;
    int image_size_y = YDIM_MAX;

    FILE *file;
    InuCommon::COsUtilities::fopen(file, filename.c_str(), "rb");

    if (file == NULL) return false;

    elements.assign(MAX_DRECT_LUT_SIZE, 0);

    int16 *rectdata = new int16[image_size_x * image_size_y * 2];
    size_t readData =
        fread(rectdata, sizeof(int16), image_size_x * image_size_y * 2, file);
    fclose(file);

    if (readData < size_t(image_size_x * image_size_y * 2)) return false;

    int grid_size_x = image_size_x / GRID_STRIDE + 1;
    int grid_size_y = image_size_y / GRID_STRIDE + 1;

    int dx, dy;

    for (int yb = 0; yb < grid_size_y - 1; yb++) {
        int y = yb * GRID_STRIDE;
        for (int xb = 0; xb < grid_size_x - 1; xb++) {
            int x = xb * GRID_STRIDE;

            int k = xb + yb * MAX_DRECT_LUT_LINE_SIZE;

            dy = rectdata[2 * x + 2 * image_size_x * y];
            dx = rectdata[2 * x + 1 + 2 * image_size_x * y];
            elements[k] =
                ((uint32)(0x00001fff & dx)) | ((uint32)(0x0000fff & dy) << 13);
        }

        int k_right =
            MAX_DRECT_LUT_LINE_SIZE - 1 + yb * MAX_DRECT_LUT_LINE_SIZE;
        dy = rectdata[2 * (image_size_x - 1) + 2 * image_size_x * y];
        dx = rectdata[2 * (image_size_x - 1) + 1 + 2 * image_size_x * y];
        elements[k_right] =
            ((uint32)(0x00001fff & dx)) | ((uint32)(0x0000fff & dy) << 13);
    }

    for (int xb = 0; xb <= grid_size_x - 1; xb++) {
        int x = xb * GRID_STRIDE;
        int k_bottom = xb + (grid_size_y - 1) * MAX_DRECT_LUT_LINE_SIZE;

        dy = rectdata[2 * x + 2 * image_size_x * (image_size_y - 1)];
        dx = rectdata[2 * x + 1 + 2 * image_size_x * (image_size_y - 1)];
        elements[k_bottom] =
            ((uint32)(0x00001fff & dx)) | ((uint32)(0x0000fff & dy) << 13);
    }
    int k_max = MAX_DRECT_LUT_LINE_SIZE - 1 +
                (grid_size_y - 1) * MAX_DRECT_LUT_LINE_SIZE;
    dy = rectdata[2 * (image_size_x - 1) +
                  2 * image_size_x * (image_size_y - 1)];
    dx = rectdata[2 * (image_size_x - 1) + 1 +
                  2 * image_size_x * (image_size_y - 1)];
    elements[k_max] =
        ((uint32)(0x00001fff & dx)) | ((uint32)(0x0000fff & dy) << 13);

    delete[] rectdata;

    return true;
}

template <class T>
bool LUTAdapter_a0::Read(const std::string &filename, std::vector<T> &elements,
                         DataType data_type, uint8 word_info_bits,
                         uint8 word_total_width_bytes) {
    uint32 temp32;
    uint64 temp64;
    typename std::vector<T>::iterator iter = elements.begin();
    uint8 word_size_bytes = word_info_bits / 8;
    uint8 elems_per_word = word_size_bytes / sizeof(T);
    const uint8 elem_size_bits = sizeof(T) * 8;

    // Pay attention that this function was loosely checked (mainly on the
    // common use case)

    if (word_info_bits != 32 && word_info_bits != 64)
        throw InvalidArgumentException("Invalid word size");

    ifstream ifs;
    if (data_type == Hex)
        ifs.open(filename.c_str(), std::ifstream::in);
    else
        ifs.open(filename.c_str(), std::ifstream::in | std::ifstream::binary);

    if (!ifs.good()) return false;

    if (data_type == Hex) {
        uint8 word_in_nibbles = word_size_bytes * 2;
        ifs >> setw(word_total_width_bytes);
        ifs >> std::hex;
        uint64 mask = CreateMask64(elem_size_bits);

        while (ifs.good()) {
            ifs.ignore(word_total_width_bytes - word_in_nibbles);
            if (word_info_bits == 64)
                ifs >> temp64;
            else {
                ifs >> temp32;
                temp64 = (uint64)temp32;
            }

            for (int k = 0; k < elems_per_word; ++k, temp64 >>= elem_size_bits)
                elements.push_back((T)(temp64 & mask));
        }
    } else {
        // Not implemented
    }

    ifs.close();

    return true;
}
