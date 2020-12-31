#ifndef _COMMON_UTILITIE_DEFS_H
#define _COMMON_UTILITIE_DEFS_H
#ifdef COMMONUTILITIES_EXPORTS
#ifdef __GNUC__
#ifndef __linux__
#define COMMONUTILITIES_API __attribute__ ((dllexport))
#else
#define COMMONUTILITIES_API __attribute__ ((visibility ("default")))
#endif
#else
#define COMMONUTILITIES_API __declspec(dllexport)
#endif
#else
#ifdef __GNUC__
#ifndef __linux__
#define COMMONUTILITIES_API __attribute__ ((dllimport))
#else
#define COMMONUTILITIES_API __attribute__ ((visibility ("default")))
#endif
#else
#define COMMONUTILITIES_API __declspec(dllimport)
#endif
#endif

#ifdef _MSC_VER
#pragma warning(disable:4251)
#endif 
#endif
