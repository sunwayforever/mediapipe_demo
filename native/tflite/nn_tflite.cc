#include "nn_tflite.h"

using namespace tflite;

NNTFLite::NNTFLite(float *feature_buffer, float *output_buffer) {
    feature_buffer_ = feature_buffer;
    output_buffer_ = output_buffer;

    const char *filename = "./model/kws.tflite";
    std::unique_ptr<tflite::FlatBufferModel> model =
        tflite::FlatBufferModel::BuildFromFile(filename);

    tflite::ops::builtin::BuiltinOpResolver resolver;
    InterpreterBuilder builder(*model, resolver);
    std::unique_ptr<Interpreter> interpreter_;
    builder(&interpreter_);
    interpreter_->AllocateTensors();
    printf("input: %s\n", interpreter_->GetInputName(0));
    printf("output: %s\n", interpreter_->GetOutputName(0));
}

void NNTFLite::Invoke() {
    float *model_input = interpreter_->typed_input_tensor<float>(0);
    for (int i = 0; i < kFeatureElementCount; i++) {
        model_input[i] = feature_buffer_[i];
    }
    interpreter_->Invoke();

}
