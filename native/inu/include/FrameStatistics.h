/*
 * File - FrameStatistics.h
 *
 * This file is part of the Inuitive SDK
 *
 * Copyright (C) 2014 All rights reserved to Inuitive
 *
 */

#ifndef __FREMESTATISTICS_H__
#define __FREMESTATISTICS_H__

#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <numeric>
#include <string>
#include <vector>

#include "BaseFrame.h"
#include "InuError.h"
#include "StreamsExport.h"

using namespace std;

namespace InuDev {

///////////////////////////////////////////////////////////////////////////////
/// \brief    Hold run time information of one element(like FW SDK or API.)
///
///  - CFrameTypeData
///
/// Responsibilities:
///      1. Measure Jitter
///      2. Measure Dropped frame
///
////////////////////////////////////////////////////////////////////////////////
class CFrameTypeData {
    friend std::ostream& operator<<(std::ostream& os,
                                    const CFrameTypeData& cftd);

   public:
    CFrameTypeData(const std::string& name);
    void AddNewSample(unsigned int index, uint64_t timeStamp) {
        mData.insert(std::pair<unsigned int, uint64_t>(index, timeStamp));
    }
    std::vector<uint64_t>& GetJitter() { return mJitter; }
    std::map<unsigned int, uint64_t>& GetSampleData() { return mData; }
    std::string& GetName() { return mName; }
    void Calculate(int iDifferenceBetweenFrameIndex);

    int GetNumOfStream() { return mNumOfStream; }
    double GetTestTime() { return mTestTime; }
    int GetDroppedFrames() { return mDroppedFrames; }
    double GetMeanJitter() { return mMeanJitter; }
    double GetStdevJitter() { return mStdevJitter; }
    double GetMinJitter() { return mMinJitter; }
    double GetMaxJitter() { return mMaxJitter; }

   private:
    std::string mName;
    std::map<unsigned int, uint64_t> mData;
    std::vector<uint64_t> mJitter;
    int mDroppedFrames;
    int mNumOfStream;
    double mTestTime;
    double mMeanJitter;
    double mStdevJitter;
    double mMinJitter;
    double mMaxJitter;
};

///////////////////////////////////////////////////////////////////////////////
/// \brief    Hold Latency information between 2 elements(like FW SDK or API.)
///
///  - CLatencyData
///
/// Responsibilities:
///      1. Measure Latency
///      2. Latency statistic
///
////////////////////////////////////////////////////////////////////////////////
class CLatencyData {
    friend std::ostream& operator<<(std::ostream& os, const CLatencyData& cld);

   public:
    CLatencyData(const std::string& name);
    void CalculateLatency(const std::map<unsigned int, uint64_t>& firstData,
                          const std::map<unsigned int, uint64_t>& secondData);

    const std::vector<int64_t>& GetLatency() const { return mLatency; }
    const std::string& GetName() const { return mName; }
    double GetMeanLatency() const { return mMeanLatency; }
    double GetStdevLatency() const { return mStdevLatency; }
    double GetMinLatency() const { return mMinLatency; }
    double GetMaxLatency() const { return mMaxLatency; }

   private:
    std::string mName;
    std::vector<int64_t> mLatency;
    double mMeanLatency;
    double mStdevLatency;
    double mMinLatency;
    double mMaxLatency;
};

struct CStatisticsData {
    CStatisticsData() : mMean(0.0), mStdev(0.0), mMin(0.0), mMax(0.0) {}
    CStatisticsData(double mean, double stdev, double min, double max)
        : mMean(mean), mStdev(stdev), mMin(min), mMax(max) {}
    double mMean;
    double mStdev;
    double mMin;
    double mMax;
};

class CFrameStatistics {
    /// \brief    stream the results
    ///
    ///
    ///
    friend std::ostream& operator<<(std::ostream& os,
                                    const CFrameStatistics& cfs);

   public:
    ////////////////////////////////////////////////////////////////////////
    /// \brief    Represents Test performance of frame handling
    ///
    /// Role: Enables to test any kind of frame performance
    ///
    /// Responsibilities:
    ///      1. Hold all elements and managed them
    ///      2. Ability to add external elements
    ///      3. Measure latency between two elements
    ///
    ////////////////////////////////////////////////////////////////////////

    /// \brief    constructor.
    ///
    /// \param[in] sensorName        Name of sensor which is check.
    /// \param[in] numOfIterations   Number of frames to measure.
    /// \param[in] waitNumOfFrame    Number of frame before test start.
    INUSTREAMS_API CFrameStatistics(const std::string& sensorName,
                                    int numOfIterations = 10000,
                                    int waitNumOfFrame = 50);

    /// \brief    Update should be call when new frame is ready.
    ///           Should be used as callback when registered to service
    ///           example to register : err =
    ///           stream->Register(std::bind(&CFrameStatistics::Update,
    ///           &CFrameStatisticsObj, std::placeholders::_2,
    ///           std::placeholders::_3));
    ///
    INUSTREAMS_API void Update(uint64_t ServiceTimestamp,
                               uint64_t StreamTimestamp, uint64_t Timestamp,
                               unsigned int FrameIndex, bool Valid,
                               InuDev::CInuError error);

    /// \brief    Update should be call when new frame is ready.
    ///           Should be use as callback when register to service
    ///           example to register : err =
    ///           stream->Register(std::bind(&CFrameStatistics::Update,
    ///           &CFrameStatisticsObj, std::placeholders::_2,
    ///           std::placeholders::_3));
    ///
    INUSTREAMS_API void Update(const InuDev::CBaseFrame& iFrame,
                               InuDev::CInuError error);

    /// \brief    DirectUpdate - same as Update,  send the values without frame.
    ///           Should be use as callback when register to service
    ///           example to register : err =
    ///           stream->Register(std::bind(&CFrameStatistics::Update,
    ///           &CFrameStatisticsObj, std::placeholders::_2,
    ///           std::placeholders::_3));
    ///
    INUSTREAMS_API void DirectUpdate(unsigned frameIndex, uint64_t fwTimestamp,
                                     uint64_t sdkTimestamp,
                                     uint64_t apiTimestamp, bool valid);

    /// \brief    signal that data is ready
    ///           will be "true" after reach to numOfIterations, and data was
    ///           calculated
    ///
    ///
    INUSTREAMS_API bool IsDataReady() { return mDataReady; }

    /// \brief    start to save frame result
    ///           use to synchronize start of all stream types (like video,
    ///           depth etc..)
    ///
    ///
    INUSTREAMS_API void StartFrameCount(int iDifferenceBetweenFrameIndex = 1);

    /// \brief    stop to save frame result
    ///
    ///
    INUSTREAMS_API void StopFrameCount();

    /// \brief    set sample which was taken from outside
    ///           ability to add external samples.
    ///
    ///
    INUSTREAMS_API void AddSampleAndData(const std::string& name,
                                         std::map<unsigned int, uint64_t> data);

    /// \brief    Calculate Latency between two time events (like SDK, API
    /// etc..)
    ///
    ///
    ///
    INUSTREAMS_API void CalcLatency(const std::string& firstName,
                                    const std::string& SecondName);

    /// \brief    getting results after measurement
    ///
    ///
    ///
    INUSTREAMS_API void GetJitter(const std::string& name,
                                  std::vector<uint64_t>& oJitter);
    INUSTREAMS_API void GetLatency(const std::string& name,
                                   std::vector<int64_t>& oLatency);
    INUSTREAMS_API int GetDroppedFrames(const std::string& name);
    INUSTREAMS_API double GetFps(const std::string& name);

    INUSTREAMS_API void GetJitterStatistics(
        const std::string& name, CStatisticsData& oStatisticsDataResults);
    INUSTREAMS_API void GetLatencyStatistics(
        const std::string& name, CStatisticsData& oStatisticsDataResults);

    INUSTREAMS_API void GetSampleTypes(std::vector<std::string>& oSampleTypes);
    INUSTREAMS_API int GetFrameCounter() { return mFrameCounter; }

    /// \brief    write results to file after measurement
    ///
    ///
    ///

    INUSTREAMS_API bool WriteResults(const std::string& filePath);
    INUSTREAMS_API const std::string& GetResultStream();

    INUSTREAMS_API bool WriteResultsSummary(const std::string& iFileName);
    INUSTREAMS_API bool WriteResultsSummary(
        const std::string& iFileName, std::vector<std::string>& SampleTypes);
    INUSTREAMS_API const std::string& GetResultsSummaryStream(
        std::vector<std::string>& SampleTypes);

    INUSTREAMS_API void StreamResultsSummary(
        std::ostream& os, std::vector<std::string>& SampleTypes);

   private:
    /// \brief    Calculate data after frames arrived.
    ///
    ///
    ///
    INUSTREAMS_API InuDev::CInuError CalculateData();

    /// \brief    signal that data is ready
    ///           will be "true" after reach to numOfIterations, and data was
    ///           calculated
    ///
    ///
    INUSTREAMS_API void AddSample(const std::string& name);

    int mFrameCounter;
    int mMumOfIterations;
    int mWaitNumOfFrame;
    int mTimeouts;
    int mDifferenceBetweenFrameIndex;

    bool mDataReady;
    bool mStartFrameCount;
    bool mStopFrameCount;
    bool mDisplayStart;
    volatile long mDataCalculated;

    std::string mStreamType;
    std::string mSummaryStr;
    std::string mResultStream;

    std::vector<CLatencyData>
        mLatencyCollection;  // collection of all calculated latency
    std::vector<std::pair<unsigned int, uint64_t>> mUpdateCollection;
    std::map<std::string, CFrameTypeData>
        mSamplesData;  // collection of CFrameTypeData which hold the samples
                       // like FW, API etc ..
};

inline void PrintHeader(std::ostream& os, const std::string& StreamType,
                        int timeouts) {
    os << std::endl;
    os << std::endl;
    os << "===================================================================="
          "==========================="
       << std::endl;
    os << "================================== Type : " << StreamType
       << " =====================================" << std::endl;
    os << "===================================================================="
          "==========================="
       << std::endl;
    os << std::endl;
    os << "Timeouts : " << timeouts << std::endl;
    os << std::endl;
}

inline void PrintOneStatistics(std::ostream& os,
                               const std::vector<uint64_t>& iVector,
                               const std::string& iLabel) {
    if (iVector.empty()) {
        os << " No elements for " << iLabel << std::endl << std::endl;
        return;
    }

    double sum = std::accumulate(iVector.begin(), iVector.end(), 0.0);
    double mean = sum / iVector.size();

    double sq_sum = std::inner_product(iVector.begin(), iVector.end(),
                                       iVector.begin(), 0.0);
    double stdev = std::sqrt(sq_sum / iVector.size() - mean * mean);

    os << iLabel << " Mean jitter: " << mean / pow(10.0, 6) << std::endl;
    os << iLabel << " Stdev jitter: " << stdev / pow(10.0, 6) << std::endl;

    os << iLabel << " Min jitter: "
       << *min_element(iVector.begin(), iVector.end()) / pow(10.0, 6)
       << std::endl;
    os << iLabel << " Max jitter: "
       << *max_element(iVector.begin(), iVector.end()) / pow(10.0, 6)
       << std::endl;

    os << std::endl;
}

inline void PrintOneHistogram(std::ostream& os,
                              const std::vector<uint64_t>& iVector,
                              const std::string& iLabel) {
    os << std::endl << iLabel << " Elements: \n";
    for (auto it = iVector.begin(); it != iVector.end(); ++it)
    //   FOR_EACH (auto& elem , iVector)
    {
        os << *it / pow(10.0, 6) << " , ";
    }

    os << std::endl;
}

inline void PrintSignedHistogram(std::ostream& os,
                                 const std::vector<int64_t>& iVector,
                                 const std::string& iLabel) {
    os << std::endl << iLabel << " Elements: \n";
    for (auto it = iVector.begin(); it != iVector.end(); ++it)
    // FOR_EACH(auto& elem , iVector)
    {
        os << *it / pow(10.0, 6) << " , ";
    }

    os << std::endl;
}

inline std::ostream& operator<<(std::ostream& os, const CFrameStatistics& cfs) {
    PrintHeader(os, cfs.mStreamType, cfs.mTimeouts);

    for (auto it = cfs.mSamplesData.begin(); it != cfs.mSamplesData.end();
         ++it) {
        os << it->second;
    }

    std::vector<CLatencyData>::const_iterator itr;
    for (itr = cfs.mLatencyCollection.begin();
         itr != cfs.mLatencyCollection.end(); itr++) {
        os << (CLatencyData)*itr;
    }

    return os;
}

inline std::ostream& operator<<(std::ostream& os, const CFrameTypeData& cftd) {
    if (cftd.mJitter.size() <=
        2)  // no jitter in case of size is smaller from 2
    {
        return os;
    }
    os << "\nStream : " << cftd.mName << " - Jitter and dropped frames"
       << std::endl;
    os << "\nResults\n========\nNumber of Frames: " << cftd.mJitter.size()
       << std::endl;
    os << "Number of dropped frames: " << cftd.mDroppedFrames << std::endl;
    os << "Number of measured frames : " << cftd.mNumOfStream << std::endl;
    os << " Test Time [sec] : " << cftd.mTestTime << std::endl;
    os << " FPS: " << (double)(cftd.mNumOfStream / cftd.mTestTime) << std::endl;
    os << cftd.mName << " Mean jitter: " << cftd.mMeanJitter << std::endl;
    os << cftd.mName << " Stdev jitter: " << cftd.mStdevJitter << std::endl;
    os << cftd.mName << " Min jitter: " << cftd.mMinJitter << std::endl;
    os << cftd.mName << " Max jitter: " << cftd.mMaxJitter << std::endl;

    PrintOneHistogram(os, cftd.mJitter, cftd.mName);
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const CLatencyData& cld) {
    os << std::endl;
    os << std::endl;
    os << cld.GetName() << " Mean Latency: " << cld.GetMeanLatency()
       << std::endl;
    os << cld.GetName() << " Stdev Latency: " << cld.GetStdevLatency()
       << std::endl;
    os << cld.GetName() << " Min Latency: " << cld.GetMinLatency() << std::endl;
    os << cld.GetName() << " Max Latency: " << cld.GetMaxLatency() << std::endl;

    // PrintSignedStatistics(os,  itr->second, itr->first);
    os << std::endl;
    PrintSignedHistogram(os, cld.GetLatency(), cld.GetName());
    return os;
}

}  // namespace InuDev

#endif
