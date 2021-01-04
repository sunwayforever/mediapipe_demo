#include "nn_tflite.h"

#include "../config.h"

NNTFLite::NNTFLite(float *feature_buffer, float *output_buffer)
    : model(tflite::FlatBufferModel::BuildFromFile(kModelFileName)),
      builder(*model, resolver),
      feature_buffer(feature_buffer),
      output_buffer(output_buffer) {
    printf("%p\n", feature_buffer);
    builder(&this->interpreter);
    this->interpreter->AllocateTensors();
}

void NNTFLite::Invoke() {
    float *model_input = this->interpreter->typed_input_tensor<float>(0);
    memcpy(model_input, this->feature_buffer, sizeof(float) * kImageSize);
    this->interpreter->Invoke();
    float *regressors = interpreter->typed_output_tensor<float>(0);
    float *classificators = interpreter->typed_output_tensor<float>(1);
    memcpy(this->output_buffer, regressors,
           kNumBoxes * kNumCoords * sizeof(float));
    memcpy(this->output_buffer + kNumBoxes * kNumCoords, classificators,
           kNumBoxes * sizeof(float));
}
