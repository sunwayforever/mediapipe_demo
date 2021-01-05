#include "util.h"
PaddingImage resize(cv::Mat img, int roi_width, int roi_height) {
    int height = img.rows;
    int width = img.cols;
    float orig_aspect_ratio = (float)height / width;
    float roi_aspect_ratio = (float)roi_height / roi_width;
    int h_padding = 0;
    int v_padding = 0;
    int new_width = 0;
    int new_height = 0;
    if (orig_aspect_ratio < roi_aspect_ratio) {
        new_width = roi_width;
        new_height = int(roi_width * orig_aspect_ratio);
        v_padding = roi_height - new_height;
    } else {
        new_height = roi_height;
        new_width = int(roi_height / orig_aspect_ratio);
        h_padding = roi_width - new_width;
    }
    cv::resize(img, img, cv::Size(new_width, new_height));
    v_padding /= 2;
    h_padding /= 2;
    cv::copyMakeBorder(img, img, v_padding, v_padding, h_padding, h_padding,
                       cv::BORDER_CONSTANT, cv::Scalar(0, 0, 0));
    return PaddingImage{
        img,
        (float)v_padding / roi_width,
        (float)h_padding / roi_height,
    };
}
