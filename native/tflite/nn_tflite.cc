#include "nn_tflite.h"

#define kTensorArenaSize 200 * 1024
static uint8_t TensorArena[kTensorArenaSize] = {0};

extern "C" void DebugLog(const char *s) { printf("%s\n", s); }

NNTFLite::NNTFLite(float *feature_buffer, float *output_buffer) {
    feature_buffer_ = feature_buffer;
    output_buffer_ = output_buffer;

    const tflite::Model *model = tflite::GetModel(g_cnn_model);
    interpreter_ =
        new tflite::MicroInterpreter(model, micro_op_resolver_, TensorArena,
                                     kTensorArenaSize, &error_reporter_);
    TfLiteStatus allocate_status = interpreter_->AllocateTensors();
    if (allocate_status != kTfLiteOk) {
        TF_LITE_REPORT_ERROR(&error_reporter_, "AllocateTensors() failed");
        return;
    }
}
void NNTFLite::Invoke() {
    TfLiteTensor *model_input = interpreter_->input(0);
    for (int i = 0; i < kFeatureElementCount; i++) {
        model_input->data.f[i] = feature_buffer_[i];
    }
    TfLiteStatus invoke_status = interpreter_->Invoke();
    if (invoke_status != kTfLiteOk) {
        TF_LITE_REPORT_ERROR(&error_reporter_, "Invoke failed");
        return;
    }
    TfLiteTensor *output = interpreter_->output(0);
    for (int i = 0; i < 2; i++) {
        output_buffer_[i] = output->data.f[i];
    }
}
