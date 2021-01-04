// 2021-01-04 14:01
#ifndef DETECTOR_H
#define DETECTOR_H
#include "Eigen/Eigen"
#include "common.h"
#include "tflite/nn_tflite.h"

struct Box {
    float score;
    float x_min, y_min;
    float w, h;
    float keypoints[kNumKeyPoints][2];
};

struct Anchor {
    float x_center, y_center;
    float w, h;
};

class Detector {
   private:
    float *nn_input;
    float *nn_output;
    NNTFLite nn;
    Anchor anchors[kNumBoxes];
    std::vector<Box> boxes;
    void gen_anchors();
    std::vector<Box> NMS(std::vector<Box>);
    std::vector<Box> Calibrate(float *raw_boxes, float *score);
    static float IOU(const Box &a, const Box &b);

   public:
    Detector();
    std::vector<Box> Detect(cv::Mat img);
};

#endif  // DETECTOR_H
