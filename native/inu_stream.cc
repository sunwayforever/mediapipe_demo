#include "inu_stream.h"

#include "video_capture.h"

VideoCapture capture;

namespace PyInuStream {
void read(uint8_t* output, int output_size) {
    // 8UC3
    cv::Mat img = capture.Read();
    memcpy(output, img.data, output_size);
}

void shape(int* height, int* width) { capture.GetShape(height, width); }
}  // namespace PyInuStream
