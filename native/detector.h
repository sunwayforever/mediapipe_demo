// 2021-01-04 14:01
#ifndef DETECTOR_H
#define DETECTOR_H
#include "Eigen/Eigen"
#include "common.h"
#include "tflite/nn_tflite.h"

class Detector {
   private:
    float *nn_input;
    float *nn_output;
    NNTFLite nn;

   public:
    Detector();
    void Detect(cv::Mat img);
};

#endif  // DETECTOR_H
