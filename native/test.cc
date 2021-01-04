#include "detector.h"
#include "video_capture.h"

int main(int argc, char *argv[]) {
    VideoCapture capture;
    Detector detector;

    cv::namedWindow("test", cv::WINDOW_NORMAL);
    while (true) {
        cv::Mat img = capture.read();
        detector.Detect(img);
        cv::imshow("test", img);
        cv::waitKey(1);
    }
}
