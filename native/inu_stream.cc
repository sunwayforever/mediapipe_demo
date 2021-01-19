#include "inu_stream.h"

#include "video_capture.h"

VideoCapture capture;

namespace PyInuStream {

void read(float* output, int output_size) {
    output[0] = 0.1234;
    // cv::Mat img = capture.read();
}

void shape(int* height, int* width) {
    *height = 1;
    *width = 2;
}

}  // namespace PyInuStream
