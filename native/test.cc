#include "detector.h"
#include "video_capture.h"

int main(int argc, char *argv[]) {
    VideoCapture capture;
    Detector detector;

    cv::namedWindow("test", cv::WINDOW_NORMAL);
    while (true) {
        cv::Mat img = capture.read();
        std::vector<Box> boxes = detector.Detect(img);
        for (auto box : boxes) {
            printf("%f\n", box.score);
        }

        cv::imshow("test", img);
        cv::waitKey(1);
    }
}
