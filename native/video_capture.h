// 2021-01-04 12:43
#ifndef VIDEO_CAPTURE_H
#define VIDEO_CAPTURE_H

#include "common.h"

class VideoCapture {
   private:
    shared_ptr<const CImageFrame> frame;
    shared_ptr<CInuSensorExt> sensor;
    shared_ptr<CImageStream> webcam_stream;
    bool init();

   public:
    VideoCapture();
    virtual ~VideoCapture();
    cv::Mat read();
};

#endif  // VIDEO_CAPTURE_H
