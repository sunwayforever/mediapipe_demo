#pragma once

#undef UNICODE

#include <stdint.h>
#include <windows.h>
#include <string>
#include <vector>
#include "CommonAlgo.h"

#define	TIFF_VERSION	        42

namespace TiffTools
{
#define	TIFFTAG_IMAGEWIDTH			256	/* image width in pixels */
#define	TIFFTAG_IMAGELENGTH			257	/* image height in pixels */
#define	TIFFTAG_ROWSPERSTRIP		278	/* rows per strip of data */
#define	TIFFTAG_SOFTWARE			305	/* name & release */
#define	TIFFTAG_IMAGEDESCRIPTION	270	/* info about image */
#define	TIFFTAG_STRIPOFFSETS		273	/* offsets to data strips */

enum TIFFDataType {
    TIFF_NOTYPE	= 0,	/* placeholder */
    TIFF_BYTE	= 1,	/* 8-bit unsigned integer */
    TIFF_ASCII	= 2,	/* 8-bit bytes w/ last byte null */
    TIFF_SHORT	= 3,	/* 16-bit unsigned integer */
    TIFF_LONG	= 4,	/* 32-bit unsigned integer */
    TIFF_RATIONAL	= 5,	/* 64-bit unsigned fraction */
    TIFF_SBYTE	= 6,	/* !8-bit signed integer */
    TIFF_UNDEFINED	= 7,	/* !8-bit untyped data */
    TIFF_SSHORT	= 8,	/* !16-bit signed integer */
    TIFF_SLONG	= 9,	/* !32-bit signed integer */
    TIFF_SRATIONAL	= 10,	/* !64-bit signed fraction */
    TIFF_FLOAT	= 11,	/* !32-bit IEEE floating point */
    TIFF_DOUBLE	= 12,	/* !64-bit IEEE floating point */
    TIFF_IFD	= 13	/* %32-bit unsigned integer (offset) */
};

// This struct contains the basic tiff header data
struct TiffHeader
{
    TiffHeader() : tiffVersion(TIFF_VERSION), // Constant tiff version
        ifdCollectionOffset(sizeof(TiffHeader)) // the offset should point to the end og the header field. after that the IFDs will be saved
    { 
        alignement[0] = 'I'; 
        alignement[1] = 'I'; 
    }

char alignement[2];
uint16_t tiffVersion;
uint32_t ifdCollectionOffset;
};

struct TiffData
{
	// Extra data
	uint16_t bitsPerSample[4];
	char     softwareName[MAX_PATH]; 
	char     imageDescription[MAX_PATH];
};


struct TiffMetaData
{
    int mWidth;
    int mHeight;
    int mImageSize;
    unsigned int mImageOffset;
    std::string mSoftwareName;
    std::string mDescription;
};

struct  TiffTag
{
    uint16_t tag;
    uint16_t tiff_type;
    uint32_t count;
    uint32_t valueOffset;
};

struct  TiffIfd
{
    TiffIfd(): numOfTags(0), nextIfd(0) {}
    uint16_t numOfTags;
    std::vector<TiffTag> ifdTags;
    uint32_t nextIfd;
};

// Reads (only the data of) files in TIFF format  and copies it to buffer
// IMPORTANT :	1.	If buffer is null, it will be allocated inside the function according to the (total) data size
//					Otherwise, buffer is assumed to be already allocated to size large enough for the data.
//				2.	If metadata is null, nothing will be returned. Otherwise, metadata assumed to be allocated and will be populated with details
bool COMMONALGO_API ReadTiff ( const std::string& fileName, BYTE*& buffer , TiffMetaData* metadata = NULL );

// Reads a file's data according to its extension : .tif/.tiff or .raw
// If extension is .tif/.tiff then width & height are returned (and obviously not used for reading)
// If extension is .raw then width & height should be provided, otherwise whole file is read 
bool COMMONALGO_API ReadTiffOrRaw_DataOnly ( const std::string& fileName, BYTE*& buffer, int* width_out = NULL, int* height_out = NULL);


} // Namespace