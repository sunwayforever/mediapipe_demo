#ifndef NN_TFLITE_H
#define NN_TFLITE_H

#include "cnn_model.h"
#include "config.h"
#include "nn.h"
#include "tensorflow/lite/micro/all_ops_resolver.h"
#include "tensorflow/lite/micro/micro_error_reporter.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"
#include "tensorflow/lite/schema/schema_generated.h"
#include "tensorflow/lite/version.h"

class NNTFLite : public NN {
   private:
    tflite::MicroInterpreter* interpreter_;
    tflite::AllOpsResolver micro_op_resolver_;
    tflite::MicroErrorReporter error_reporter_;

    float* feature_buffer_;
    float* output_buffer_;

   public:
    NNTFLite(float* feature_buffer, float* output_buffer);

    void Invoke();
};

#endif
