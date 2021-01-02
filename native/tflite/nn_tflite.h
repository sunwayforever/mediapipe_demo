#ifndef NN_TFLITE_H
#define NN_TFLITE_H
#include <unistd.h>

#include <cstdio>

#include "tensorflow/lite/interpreter.h"
#include "tensorflow/lite/kernels/register.h"
#include "tensorflow/lite/model.h"
#include "tensorflow/lite/optional_debug_tools.h"

#define kFeatureElementCount 400

class NNTFLite {
   private:
    tflite::Interpreter* interpreter_;

    float* feature_buffer_;
    float* output_buffer_;

   public:
    NNTFLite(float* feature_buffer, float* output_buffer);

    void Invoke();
};

#endif
