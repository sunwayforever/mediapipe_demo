#include "detector.h"
#include "video_capture.h"

void AnnotateImage(cv::Mat img, std::vector<Box> boxes) {
    int height = img.rows;
    int width = img.cols;
    for (auto &box : boxes) {
        int x1 = box.x_min * width;
        int x2 = (box.x_min + box.w) * width;
        int y1 = box.y_min * height;
        int y2 = (box.y_min + box.h) * height;
        cv::rectangle(img, cv::Point(x1, y1), cv::Point(x2, y2), cv::Scalar(255, 0, 0), 1);
    }
}

int main(int argc, char *argv[]) {
    VideoCapture capture;
    Detector detector;

    cv::namedWindow("test", cv::WINDOW_NORMAL);
    while (true) {
        cv::Mat img = capture.read();
        std::vector<Box> boxes = detector.Detect(img);
        AnnotateImage(img, boxes);
        cv::imshow("test", img);
        cv::waitKey(1);
    }
}
