// 2021-01-04 17:33
#ifndef RAW_BOX_H
#define RAW_BOX_H
#include "common.h"

class RawBox {
   public:
    float x_center, y_center;
    float w, h;
    cv::Vec2f keypoints[kNumKeyPoints];
};

#endif  // RAW_BOX_H
