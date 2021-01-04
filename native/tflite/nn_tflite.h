#ifndef NN_TFLITE_H
#define NN_TFLITE_H
#include <unistd.h>

#include <cstdio>

#include "tensorflow/lite/interpreter.h"
#include "tensorflow/lite/kernels/register.h"
#include "tensorflow/lite/model.h"
#include "tensorflow/lite/optional_debug_tools.h"
using namespace tflite;

class NNTFLite {
   private:
    std::unique_ptr<Interpreter> interpreter;
    std::unique_ptr<FlatBufferModel> model;
    ops::builtin::BuiltinOpResolver resolver;
    InterpreterBuilder builder;

    float* feature_buffer;
    float* output_buffer;

   public:
    NNTFLite(float* feature_buffer, float* output_buffer);

    void Invoke();
};

#endif
