///////////////////////////////////
//common types
#pragma once

#include <stdint.h>
#include <cmath>

#define RUN_PARALLEL


#define MAX_BIT_DEPTH 16
#define XDIM_720P 1280
#define YDIM_720P 720
#define YDIM_960P 960

#define XDIM_MAX 1920
#define YDIM_MAX 1080

// Definitions from DRect.h (NU3000Sim_a0)
#define MAX_GRID_SIZE_X 256
#define MAX_GRID_SIZE_Y 128
#define GRID_STRIDE 16
#define MAX_DRECT_LUT_SIZE 8448
#define MAX_DRECT_LUT_LINE_SIZE (XDIM_MAX/ GRID_STRIDE + 1)
#define NUM_DRECT_LUT_LINES (YDIM_MAX/ GRID_STRIDE + 1)
///////////////////////////////////////////////

// Definitions from IntensityBalance.h (NU3000Sim_a0)
#define INTENSITY_GRID_DIMX 256
#define INTENSITY_GRID_DIMY 128
#define INTENSITY_GRID_STRIDE 16
#define MAX_IB_LUT_SIZE 8352
#define MAX_IB_LUT_LINE_SIZE (int)(std::ceil(XDIM_MAX / float(INTENSITY_GRID_STRIDE)) + 1)
#define MAX_IB_LUT_LINES_NUM (int)(std::ceil(YDIM_MAX / float(INTENSITY_GRID_STRIDE)) + 1)

#define LUT_SIZE_FR_X 81
#define LUT_SIZE_FR_Y 61
#define LUT_SIZE_BINNED_X 41
#define LUT_SIZE_BINNED_Y 31

// How many bits of information per channel are used in the LUT
#define IB_LUT_CHANNEL_BITS 10
///////////////////////////////////////////////

typedef char int8 ;
typedef unsigned char uint8 ;
typedef  short int16;
typedef unsigned short  uint16;
typedef int int32;
typedef unsigned int uint32;
typedef  int64_t int64;
typedef  uint64_t uint64;

#ifndef MAX
	#define MAX(a,b) (a)>(b) ? (a) : (b)
#endif
#ifndef MIN
	#define MIN(a,b) (a)<(b) ? (a) : (b)
#endif

template<class T>
struct int_vec
{
	T x;
	T y;
};

typedef int_vec<int32> int_vec32;


struct wb_struct{
	int r_gain;
	int g_gain;
	int b_gain;
	int r_off;
	int g_off;
	int b_off;
	int black_level_off;
};


