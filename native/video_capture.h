// 2021-01-04 12:43
#ifndef VIDEO_CAPTURE_H
#define VIDEO_CAPTURE_H

#include "common.h"

class VideoCapture {
   private:
    shared_ptr<const CImageFrame> image_frame;
    shared_ptr<const CImageFrame> depth_frame;
    shared_ptr<CInuSensorExt> sensor;
    shared_ptr<CImageStream> webcam_stream;
    shared_ptr<CDepthStream> depth_stream;
    bool init();

   public:
    VideoCapture();
    virtual ~VideoCapture();
    cv::Mat ReadBGRImage();
    cv::Mat ReadDepthImage();
    void GetShape(int *height, int *width);
};

#endif  // VIDEO_CAPTURE_H
