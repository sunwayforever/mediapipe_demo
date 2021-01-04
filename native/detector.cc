#include "detector.h"

Detector::Detector()
    : nn_input(new float[kImageSize]),
      nn_output(new float[kOutputSize]),
      nn(nn_input, nn_output) {}

void Detector::Detect(cv::Mat img) {
    // convert to rgb
    cv::cvtColor(img, img, cv::COLOR_BGR2RGB);
    // resize
    cv::resize(img, img, cv::Size(kImageWidth, kImageHeight));
    // convert 8UC3 to float32 (-1,1)
    img.convertTo(img, CV_32F, 1.0 / 127.5, -1);
    int total_size = img.cols * img.rows * img.elemSize();
    memcpy(this->nn_input, img.data, total_size);
    this->nn.Invoke();
    // float *regressors = this->nn_output;
    // float *classificators = this->nn_output + kNumBoxes * kNumCoords;
}
