
#pragma once 
#include <string>
#include <map>
#include <string>

using namespace std;

typedef enum {
	DEFAULT = -1,
	Y8 = 0,
	Y12,
	Y16,
	YUV_420_16_COMPACT,
	YUV_422_8,
	YUV_1288,
	YUV8,
	RGB8,
	RGB16,
	BYR8,
	BYR10,
	BYR10SW,
	NUM_OF_PIXEL_FORMATS
} PixelFormatType;


const std::string PixelFormatString[] = 
{
	"Y8",
	"Y12",
	"Y16",
	"YUV_420_16_COMPACT",
	"YUV_422_8",
	"YUV_1288",
	"YUV8",
	"RGB8",
	"RGB16",
	"BYR8",
	"BYR10",
	"BYR10SW"
};

const int BytesPerPixel[NUM_OF_PIXEL_FORMATS] = {1, 2, 2, 2, 2, 4, 3, 3, 6, 1, 2, 2};
const int NibblesPerPixel[NUM_OF_PIXEL_FORMATS] = {2, 3, 4, 4, 4, 7, 6, 6, 12, 2, 4, 4};

const std::string HexOutWidth[] = {"%x","%01x","%02x","%03x","%04x","%05x","%06x","%07x","%08x","%09x","%10x","%11x","%12x"};

typedef enum {
	PIXEL_DATA,
	DISPARITY_DATA,
	OTHER_DATA,
} StreamDataType;


const double  RGB2YUV_MAT[3][4] =  {{0.257, 0.504, 0.098, 16.0},			// Y
									{-0.148, -0.291, 0.439, 128.0},			// Cb = U
									{0.439, -0.368, -0.071, 128.0}};		// Cr = V


	

const double  YUV2RGB_MAT[3][4] =  {{1.164, 0, 1.596, -222.91},			//R
									{1.164, -0.392, -0.813, 135.576},	   // G
									{1.164, 2.017, 0, -276.836}};			// B


#ifndef MAX
#define MAX(a,b) ((a>b) ? a : b)
#endif
#ifndef MIN
#define MIN(a,b) ((a<b) ? a : b)
#endif

inline void yuv2rgb_convert (unsigned char * pRGB, unsigned char* pYUV, const double conv_matr[3][4])
{
	double r = (unsigned char)(*pYUV*conv_matr[0][0]	+	*(pYUV+1)*conv_matr[0][1]	+	*(pYUV+2)*conv_matr[0][2]	+	conv_matr[0][3]);	
	double g = (unsigned char)(*pYUV*conv_matr[1][0]	+	*(pYUV+1)*conv_matr[1][1]	+	*(pYUV+2)*conv_matr[1][2]	+	conv_matr[1][3]);
	double b = (unsigned char)(*pYUV*conv_matr[2][0]	+	*(pYUV+1)*conv_matr[2][1]	+	*(pYUV+2)*conv_matr[2][2]	+	conv_matr[2][3]);


	*(pRGB+0)	= (unsigned char)(MAX (0, MIN(b, 255.0)));
	*(pRGB+1)	= (unsigned char)(MAX (0, MIN(g, 255.0)));
	*(pRGB+2)	= (unsigned char)(MAX (0, MIN(r, 255.0)));

}

typedef enum {UNKNOWN_TYPE = -1, HD_1FPS, HD_10FPS, FULLRES_1FPS, FULLRES_2FPS, FULLRES_3FPS, FULLRES_10FPS, VGA_1FPS, VGA_3FPS, VGA_10FPS, VGA_20FPS, VGA_SYNTH_1FPS, VGA_SYNTH_10FPS, QVGA_60FPS, NUM_TABLE_PRESETS} PresetTableType;
typedef enum {HD, FR, VGA, QVGA} InputFormat;
const InputFormat  FrameFormats[NUM_TABLE_PRESETS] = {HD, HD, FR, FR, FR, FR, VGA, VGA, VGA, VGA, VGA, VGA, QVGA};
const std::string  sides[2] = {"left", "right"};
const std::string frame_formats[3] = {"_hd", "_fr", "_vga"};
typedef enum {GRBG, RGGB, BGGR, GBRG} BayerPattern;
typedef enum {RED_BYR, GREEN_BYR, BLUE_BYR} BayerColor;

const BayerColor PatternColor[4][2][2] = 
{
	{{GREEN_BYR, RED_BYR}, {BLUE_BYR, GREEN_BYR}},
	{{RED_BYR, GREEN_BYR}, {GREEN_BYR, BLUE_BYR}},
	{{BLUE_BYR, GREEN_BYR}, {GREEN_BYR, RED_BYR}},
	{{GREEN_BYR, BLUE_BYR}, {RED_BYR, GREEN_BYR}}
};

typedef enum {
	FULL_FILE = 0,
	EMPTY_FILE,
	ONLY_HEADER_FILE
} CIIFFileOutMode;

