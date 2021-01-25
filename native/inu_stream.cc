#include "inu_stream.h"

#include "video_capture.h"

VideoCapture capture;

namespace PyInuStream {
void read_bgr_image(uint8_t* output, int output_size) {
    // 8UC3
    cv::Mat img = capture.ReadBGRImage();
    memcpy(output, img.data, output_size);
}

void read_depth_image(uint8_t* output, int output_size) {
    // 16U
    cv::Mat img = capture.ReadDepthImage();
    memcpy(output, img.data, output_size);
}

void shape(int* height, int* width) { capture.GetShape(height, width); }
}  // namespace PyInuStream
