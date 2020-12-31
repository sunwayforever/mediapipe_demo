/*
* File - Version.h
*
* This file is part of the Inuitive SDK
*
* Copyright (C) 2014 All rights reserved to Inuitive
*
*/

#define IAF_VERSION_1  4
#define IAF_VERSION_2  09
#define IAF_VERSION_3  0016
#define IAF_VERSION_4  174

#define IAF_VERSION_1_STR  "4"
#define IAF_VERSION_2_STR  "09"
#define IAF_VERSION_3_STR  "0016"
#define IAF_VERSION_4_STR  "174"

#define IAF_VERSION_STR "4.09.0016.174"

#define IAF_PRODUCT_1  IAF_VERSION_1
#define IAF_PRODUCT_2  IAF_VERSION_2
#define IAF_PRODUCT_3  IAF_VERSION_3
#define IAF_PRODUCT_4  IAF_VERSION_4

#define IAF_PRODUCT_STR IAF_VERSION_STR

#include <string>
#include <sstream>
#include <iomanip>

inline int MyAtoi(char* str)  
{
    std::istringstream tmp(str);
    int ret;
    tmp >> ret;
    return ret;
}

static const unsigned int IAF_VERSION_NUMBER(10000000 * MyAtoi((char*)IAF_VERSION_1_STR) + 100000 * MyAtoi((char*)IAF_VERSION_2_STR) + 1000 * MyAtoi((char*)IAF_VERSION_3_STR) + MyAtoi((char*)IAF_VERSION_4_STR));

