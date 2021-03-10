#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# 2021-02-20 12:41
import math
import numpy as np

from collections import namedtuple

from common import util
from common.detector import Detector
from message_broker.transport import Publisher

BoxConfig = namedtuple(
    "BoxConfig",
    [
        "model",
        "img_height",
        "img_width",
        "num_coords",
        "num_boxes",
        "min_score_thresh",
        "nms_thresh",
        "num_keypoint",
    ],
)


class Anchor:
    def __init__(self, x_center, y_center, h, w):
        self.x_center = x_center
        self.y_center = y_center
        self.h = h
        self.w = w


class Box:
    def __init__(self, score, box, num_key_points):
        self.score = score
        self.xmin = box[0]
        self.ymin = box[1]
        self.width = box[2]
        self.height = box[3]
        self.keypoints = []
        for i in range(num_key_points):
            self.keypoints.append([box[4 + 2 * i], box[4 + 2 * i + 1]])


class BoxDetector(Detector):
    def __init__(self, config: BoxConfig):
        super().__init__(
            config.model,
            {
                "tflite": [0, 1],
                "onnx": ["regressors", "classificators"],
                "tf": ["output_0", "output_1"],
            },
        )
        self.config = config
        self.anchors = self._gen_anchors()
        self.publisher = Publisher()

    def detect(self, img):
        self.img_width = img.shape[1]
        self.img_height = img.shape[0]
        # img_rgb = np.stack([img_rgb, img_rgb, img_rgb], axis=2)
        # [[file:~/source/mediapipe/mediapipe/modules/face_detection/face_detection_front_cpu.pbtxt::keep_aspect_ratio: true]]
        input_data, v_padding, h_padding = util.resize(
            img, self.config.img_width, self.config.img_height
        )
        input_data = (input_data - 127.5) / 127.5
        input_data = np.expand_dims(input_data, axis=0)
        input_data = input_data.astype(np.float32)

        regressors, classificators = super().invoke(input_data)

        boxes = self._post_detect(regressors, classificators)

        restore_x = (
            lambda x: (x - h_padding)
            * self.config.img_width
            * self.img_width
            / ((1 - 2 * h_padding) * self.config.img_width)
        )
        restore_y = (
            lambda y: (y - v_padding)
            * self.config.img_height
            * self.img_height
            / ((1 - 2 * v_padding) * self.config.img_height)
        )
        restore_width = (
            lambda w: w
            * self.config.img_width
            * self.img_width
            / ((1 - 2 * h_padding) * self.config.img_width)
        )
        restore_height = (
            lambda h: h
            * self.config.img_height
            * self.img_height
            / ((1 - 2 * v_padding) * self.config.img_height)
        )
        for box in boxes:
            box.xmin = int(restore_x(box.xmin))
            box.ymin = int(restore_y(box.ymin))
            box.width = int(restore_width(box.width))
            box.height = int(restore_height(box.height))
            for point in box.keypoints:
                point[0] = int(restore_x(point[0]))
                point[1] = int(restore_y(point[1]))

        # 最终输出坐标是 webcam image 上的绝对坐标
        return boxes

    def _post_detect(self, regressors, classificators):
        raw_boxes = np.reshape(regressors, (-1))
        raw_scores = np.reshape(classificators, (-1))

        raw_scores = util.sigmoid(np.clip(raw_scores, -100, 100))
        boxes = []
        for i in range(self.config.num_boxes):
            if raw_scores[i] < self.config.min_score_thresh:
                continue
            box = self._calibrate_box(raw_boxes, i)
            box = Box(raw_scores[i], box, self.config.num_keypoint)
            boxes.append(box)
        return self._NMS(boxes)

    def _gen_anchors(self):
        anchors = []
        for stride, count in zip([8, 16], [1, 3]):
            feature_map_height = math.ceil(1.0 * self.config.img_height / stride)
            feature_map_width = math.ceil(1.0 * self.config.img_width / stride)

            for y in range(feature_map_height):
                for x in range(feature_map_width):
                    for _ in range(2 * count):
                        x_center = (x + 0.5) / feature_map_width
                        y_center = (y + 0.5) / feature_map_height
                        w = 1.0
                        h = 1.0
                        new_anchor = Anchor(x_center, y_center, h, w)
                        anchors.append(new_anchor)

        return anchors

    def _NMS(self, boxes):
        # Not Max Supression
        if len(boxes) <= 0:
            return np.array([])
        box = np.array(
            [
                [d.xmin, d.xmin + d.width, d.ymin, d.ymin + d.height, d.score]
                for d in boxes
            ]
        )

        x1 = np.array(box[:, 0])
        x2 = np.array(box[:, 1])
        y1 = np.array(box[:, 2])
        y2 = np.array(box[:, 3])
        score = np.array(box[:, 4])

        area = np.multiply(x2 - x1 + 1, y2 - y1 + 1)
        I = np.array(score.argsort())  # read score using I
        pick = []
        while len(I) > 0:
            best = I[-1]
            pick.append(best)
            # 计算 box 中 best (I[-1]) 与所有其它 box (I[0:-1]) IOU (Intersection
            # Over Union), 过滤掉 IOU >= NMS_THRESH 的 box, 因为它们与当前 best 有很
            # 大的重叠
            xx1 = np.maximum(x1[best], x1[I[0:-1]])
            yy1 = np.maximum(y1[best], y1[I[0:-1]])
            xx2 = np.minimum(x2[best], x2[I[0:-1]])
            yy2 = np.minimum(y2[best], y2[I[0:-1]])
            w = np.maximum(0.0, xx2 - xx1 + 1)
            h = np.maximum(0.0, yy2 - yy1 + 1)
            intersection = w * h
            iou = intersection / (area[best] + area[I[0:-1]] - intersection)
            I = I[np.where(iou <= self.config.nms_thresh)[0]]
        return [boxes[i] for i in pick]

    def _calibrate_box(self, raw_boxes, index):
        box_data = np.zeros(self.config.num_coords)
        box_offset = index * self.config.num_coords

        # raw_box 的 x_center,y_center 是 box 中心坐标距其对应的 anchor 中心坐标的绝
        # 对偏移量与 anchor 的 scale 的比值. w,h 是 box 的绝对大小与 anchor 的 scale
        # 的比值.因为考虑了 anchor 的 scale, 不同大小的 anchor 会以相同的`比例`
        # capture 它附近的 box在 MediaPipe 的代码中, anchor 的 w,h 均为 1.0, 所以它
        # 对特别小的物体效果不佳
        x_center = raw_boxes[box_offset]
        y_center = raw_boxes[box_offset + 1]
        w = raw_boxes[box_offset + 2]
        h = raw_boxes[box_offset + 3]

        # x_center, y_center 转换为 box 的绝对中心坐标 (0~1)
        x_center = (
            x_center / self.config.img_width * self.anchors[index].w
            + self.anchors[index].x_center
        )
        y_center = (
            y_center / self.config.img_height * self.anchors[index].h
            + self.anchors[index].y_center
        )

        # h, w 变为 box 的绝对大小 (0~1)
        h = h / self.config.img_height * self.anchors[index].h
        w = w / self.config.img_width * self.anchors[index].w

        box_data[0] = x_center - w / 2.0
        box_data[1] = y_center - h / 2.0
        box_data[2] = w
        box_data[3] = h

        # for 6 keypoint
        for j in range(self.config.num_keypoint * 2):
            box_data[4 + j] = raw_boxes[box_offset + 4 + j]
            if j % 2 == 0:
                box_data[4 + j] = (
                    box_data[4 + j] / self.config.img_width
                    + self.anchors[index].x_center
                )
            else:
                box_data[4 + j] = (
                    box_data[4 + j] / self.config.img_height
                    + self.anchors[index].y_center
                )

        return box_data
