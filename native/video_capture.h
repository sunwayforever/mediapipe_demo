// 2021-01-04 12:43
#ifndef VIDEO_CAPTURE_HH
#define VIDEO_CAPTURE_HH

#include "common.h"

class VideoCapture {
   private:
    const static int kFrameRate = 60;
    shared_ptr<const CImageFrame> frame;
    shared_ptr<CInuSensorExt> sensor;
    shared_ptr<CImageStream> webcam_stream;
    bool init();

   public:
    VideoCapture();
    virtual ~VideoCapture();
    cv::Mat read();
};

#endif  // VIDEO_CAPTURE_HH
