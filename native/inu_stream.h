// 2021-01-19 18:04
#ifndef INU_STREAM_H
#define INU_STREAM_H
#include <stdint.h>

namespace PyInuStream {
void read_bgr_image(uint8_t *output, int output_size);
void read_depth_image(uint8_t *output, int output_size);
void shape(int *height, int *width);
}  // namespace PyInuStream

#endif  // INU_STREAM_H
