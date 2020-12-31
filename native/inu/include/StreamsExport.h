/*
 * File - StreamsExport.h
 *
 * This file is part of the Inuitive SDK
 *
 * Copyright (C) 2014 All rights reserved to Inuitive
 *
 */

#ifndef __STREAMSEXPORT_H__
#define __STREAMSEXPORT_H__

#ifdef INUSTREAMS_EXPORTS
    #ifdef __GNUC__
        #ifdef __linux__
            #define INUSTREAMS_API __attribute__((visibility("default")))
        #else
            #define INUSTREAMS_API __attribute__ ((visibility ("default")))
        #endif
    #else
        #define INUSTREAMS_API __declspec(dllexport)
    #endif
    #define INUSTREAMS_API_TEMPLATE
#else
    #ifdef __GNUC__
        #ifdef __linux__
            #define INUSTREAMS_API
        #else
            #define INUSTREAMS_API __attribute__ ((visibility ("default")))
        #endif
    #else
        #define INUSTREAMS_API __declspec(dllimport)
    #endif
    #define INUSTREAMS_API_TEMPLATE extern
#endif

#endif //__STREAMSEXPORT_H__
