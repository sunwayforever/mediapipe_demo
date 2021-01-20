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
        cv::rectangle(img, cv::Point(x1, y1), cv::Point(x2, y2),
                      cv::Scalar(0, 255, 0), 2);
        std::ostringstream os;
        os.precision(2);
        os << box.score;
        cv::putText(img, os.str(), cv::Point(x1, y1 - 6), 0, 1,
                    cv::Scalar(0, 255, 0));

        for (auto &point : box.keypoints) {
            cv::circle(img, cv::Point(point[0] * width, point[1] * height), 1,
                       cv::Scalar(0, 0, 255), 2);
        }
    }
}

int main(int argc, char *argv[]) {
    VideoCapture capture;
    Detector detector;

    cv::namedWindow("test", cv::WINDOW_NORMAL);
    while (true) {
        cv::Mat img = capture.Read();
        std::vector<Box> boxes = detector.Detect(img);
        AnnotateImage(img, boxes);
        cv::imshow("test", img);
        if ((cv::waitKey(1) & 0xff) == 0x71) {
            break;
        }
    }
    cv::destroyWindow("test");
}
