// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the COMMONALGO_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// COMMONALGO_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.

#ifndef __COMMON_ALGO_H__
#define __COMMON_ALGO_H__

#ifdef COMMONALGO_EXPORTS
    #ifdef __GNUC__
		#ifndef __linux__
			#define COMMONALGO_API __attribute__ ((dllexport))
		#else 
			#define COMMONALGO_API __attribute__ ((visibility ("default")))
		#endif
	#else
		#define COMMONALGO_API __declspec(dllexport)
	#endif
    #define COMMONALGO_API_TEMPLATE
#else
    #ifdef __GNUC__
		#ifndef __linux__
			#define COMMONALGO_API
		#else
			#define COMMONALGO_API
		#endif
	#else
		#define COMMONALGO_API __declspec(dllimport)
	#endif
    #define COMMONALGO_API_TEMPLATE extern
#endif

#ifdef __cplusplus
namespace  InuCommon
{
#endif
	// Alpha transparent value (in cased of RGBA format)
	static const unsigned char ALPHA_TRANSPARENT = 0xff;
#ifdef __cplusplus
}
#endif

#endif