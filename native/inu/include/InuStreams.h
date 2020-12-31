/*
 * File - InuStreams.h
 *
 * This file is part of the Inuitive SDK
 *
 * Copyright (C) 2014 All rights reserved to Inuitive
 *
 */

#ifndef __INUSTREAMS_H__
#define __INUSTREAMS_H__


////////////////////////////////////////////////////////////////////////
///
/// Includes all InuDev streams (easier inclusion into projects)
///
////////////////////////////////////////////////////////////////////////

// Role: Creates and provides NUI streams objects
#include "InuSensor.h"

// Role: Interface for InuDev Video Streaming Service.
#include "StereoImageStream.h"

// Role: Interface for InuDev Depth streaming Service.
#include "DepthStreamExt.h"

// Role: Interface for InuDev Image streaming Service.
#include "ImageStreamExt.h"

// Role: Interface for InuDev General Purpose streaming Service.
#include "UserDefinedStream.h"

// Role: Interface for InuDev Head streaming Service.
#include "FeaturesTrackingStream.h"

// Role: Interface for InuDev IMU streaming Service.
#include "ImuStream.h"

// Role: Interface for InuDev Audio streaming Service.
#include "AudioStream.h"

// Role: Interface for InuDev SLAM streaming Service.
#include "SlamStream.h"

// Role: Interface for InuDev Point Cloud streaming Service.
#include "PointCloudStream.h"

// Role: Interface for InuDev Injection streaming Service.
#include "InjectionStream.h"

// Role: Interface for InuDev Histogram streaming Service.
#include "HistogramStream.h"

// Role: Interface for InuDev Voxels streaming Service.
#include "VoxelsStream.h"

#endif // __INUSTREAMS_H__

