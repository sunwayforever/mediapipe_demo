#include "detector.h"

Detector::Detector()
    : nn_input(new float[kImageSize]),
      nn_output(new float[kOutputSize]),
      nn(nn_input, nn_output) {
    gen_anchors();
}

Eigen::ArrayXf sigmoid(Eigen::ArrayXf input) {
    return 1.0 / (1.0 + 1.0 / input.exp());
}

void Detector::gen_anchors() {
    // stide = 8
    int height = std::ceil(1.0 * kImageHeight / 8);
    int width = std::ceil(1.0 * kImageWidth / 8);

    int anchor_index = 0;
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            for (int i = 0; i < 2; i++) {
                float x_center = (x + 0.5) / width;
                float y_center = (y + 0.5) / height;
                float w = 1.0;
                float h = 1.0;
                this->anchors[anchor_index++] =
                    Anchor{x_center, y_center, w, h};
            }
        }
    }
    // stide = 16
    height = std::ceil(1.0 * kImageHeight / 16);
    width = std::ceil(1.0 * kImageWidth / 16);

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            for (int i = 0; i < 6; i++) {
                float x_center = (x + 0.5) / width;
                float y_center = (y + 0.5) / height;
                float w = 1.0;
                float h = 1.0;
                this->anchors[anchor_index++] =
                    Anchor{x_center, y_center, w, h};
            }
        }
    }
}

vector<Box> Detector::Detect(cv::Mat input_img) {
    cv::Mat img;
    // convert to rgb
    cv::cvtColor(input_img, img, cv::COLOR_BGR2RGB);
    // resize
    PaddingImage padding_img = resize(img, kImageWidth, kImageHeight);
    img = padding_img.img;
    float h_padding = padding_img.h_padding;
    float v_padding = padding_img.v_padding;
    // convert 8UC3 to float32 (-1,1)
    img.convertTo(img, CV_32F, 1.0 / 127.5, -1);
    int total_size = img.cols * img.rows * img.elemSize();
    memcpy(this->nn_input, img.data, total_size);
    this->nn.Invoke();
    float *raw_boxes = this->nn_output;
    Eigen::Map<Eigen::ArrayXf> scores(this->nn_output + kNumBoxes * kNumCoords,
                                      kNumBoxes);

    scores = sigmoid(scores);

    auto restore_x = [h_padding](float x) -> float {
        return (x - h_padding) * kImageWidth /
               ((1 - 2 * h_padding) * kImageWidth);
    };
    auto restore_y = [v_padding](float y) -> float {
        return (y - v_padding) * kImageHeight /
               ((1 - 2 * v_padding) * kImageHeight);
    };
    auto restore_width = [h_padding](float w) -> float {
        return w * kImageWidth / ((1 - 2 * h_padding) * kImageWidth);
    };
    auto restore_height = [v_padding](float h) -> float {
        return h * kImageHeight / ((1 - 2 * v_padding) * kImageHeight);
    };

    std::vector<Box> boxes = NMS(Calibrate(raw_boxes, scores.data()));
    for (auto &box : boxes) {
        box.x_min = restore_x(box.x_min);
        box.y_min = restore_y(box.y_min);
        box.w = restore_width(box.w);
        box.h = restore_height(box.h);
        for (auto &point : box.keypoints) {
            point[0] = restore_x(point[0]);
            point[1] = restore_y(point[1]);
        }
    }
    return boxes;
}

std::vector<Box> Detector::Calibrate(float *raw_boxes, float *scores) {
    std::vector<Box> ret;
    for (int i = 0; i < kNumBoxes; i++) {
        float *raw_box = raw_boxes + i * kNumCoords;
        Anchor &anchor = this->anchors[i];
        float score = scores[i];
        if (score < kMinScoreThresh) {
            continue;
        }
        float x_center = raw_box[0];
        float y_center = raw_box[1];
        float w = raw_box[2];
        float h = raw_box[3];
        x_center = x_center / kImageWidth * anchor.w + anchor.x_center;
        y_center = y_center / kImageHeight * anchor.h + anchor.y_center;
        w = w / kImageWidth * anchor.w;
        h = h / kImageHeight * anchor.h;

        Box box{score, x_center - w / 2.0, y_center - h / 2.0, w, h};
        for (int i = 0; i < kNumKeyPoints; i++) {
            box.keypoints[i][0] =
                raw_box[4 + i * 2] / kImageWidth + anchor.x_center;
            box.keypoints[i][1] =
                raw_box[4 + i * 2 + 1] / kImageHeight + anchor.y_center;
        }
        ret.push_back(box);
    }
    return ret;
}

vector<Box> Detector::NMS(vector<Box> boxes) {
    vector<Box> picked;
    std::sort(
        boxes.begin(), boxes.end(),
        [](const Box &a, const Box &b) -> bool { return a.score > b.score; });

    vector<bool> supressed(boxes.size(), false);
    for (int i = 0; i < boxes.size(); i++) {
        if (supressed[i]) {
            continue;
        }
        Box &best = boxes[i];
        picked.push_back(best);
        for (int j = i + 1; j < boxes.size(); j++) {
            float iou = IOU(best, boxes[j]);
            if (iou >= 0.5) {
                supressed[j] = true;
            }
        }
    }
    return picked;
}

float Detector::IOU(const Box &a, const Box &b) {
    float x1 = std::max(a.x_min, b.x_min);
    float y1 = std::max(a.y_min, b.y_min);
    float x2 = std::min(a.x_min + a.w, b.x_min + b.w);
    float y2 = std::min(a.y_min + a.h, b.y_min + b.h);
    float w = std::max(0.0f, x2 - x1);
    float h = std::max(0.0f, y2 - y1);
    float intersection = w * h;
    return intersection / (a.w * a.h + b.w * b.h - intersection);
}
