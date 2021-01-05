// 2021-01-05 07:10
#ifndef UTIL_H
#define UTIL_H

#include "common.h"

struct ResizedImage {
    cv::Mat img;
    float v_padding;
    float h_padding;
};

ResizedImage ResizeAndKeepAspectRatio(cv::Mat img, int roi_width,
                                      int roi_height);
#endif  // UTIL_H
