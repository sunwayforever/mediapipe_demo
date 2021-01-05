// 2021-01-04 16:27
#ifndef CONFIG_H
#define CONFIG_H

#define kMinScoreThresh 0.68
#define kNMSThresh 0.5
#define kNumBoxes 896
#define kNumCoords 16
#define kNumKeyPoints 6
#define kOutputSize kNumBoxes*(kNumCoords + 1)
#define kImageHeight 128
#define kImageWidth 128
#define kImageSize kImageHeight* kImageWidth * 3
#define kModelFileName \
    "/home/sunway/source/mediapipe-demo/model/face_detection_front.tflite"

#endif  // CONFIG_H
