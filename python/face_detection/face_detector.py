#!/usr/bin/env python3
# -*- coding: utf-8 -*-
import math
import cv2
import numpy as np
import MNN

from .config import *
from message_broker import Publisher

from common import util


def NMS(boxes):
    if len(boxes) <= 0:
        return np.array([])
    box = np.array(
        [[d.xmin, d.xmin + d.width, d.ymin, d.ymin + d.height, d.score] for d in boxes]
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
        xx1 = np.maximum(x1[best], x1[I[0:-1]])
        yy1 = np.maximum(y1[best], y1[I[0:-1]])
        xx2 = np.minimum(x2[best], x2[I[0:-1]])
        yy2 = np.minimum(y2[best], y2[I[0:-1]])
        w = np.maximum(0.0, xx2 - xx1 + 1)
        h = np.maximum(0.0, yy2 - yy1 + 1)
        intersection = w * h
        iou = intersection / (area[best] + area[I[0:-1]] - intersection)
        I = I[np.where(iou <= NMS_THRESH)[0]]
    return [boxes[i] for i in pick]


class Box:
    def __init__(self, score, box, keypoints):
        self.score = score
        self.xmin = box[0]
        self.ymin = box[1]
        self.width = box[2]
        self.height = box[3]
        self.keypoints = keypoints

    def __str__(self):
        return f"{self.xmin, self.ymin,self.width, self.height}"


class Anchor:
    def __init__(self, x_center, y_center, h, w):
        self.x_center = x_center
        self.y_center = y_center
        self.height = h
        self.width = w


def gen_anchors():
    anchors = []
    min_size = {10, 16, 24}, {32, 48}, {64, 96, 128}
    for stride, box_size in zip([8, 16, 32], [[10, 16, 24], [32, 48], [64, 96, 128]]):
        feature_map_height = math.ceil(IMG_HEIGHT / stride)
        feature_map_width = math.ceil(IMG_WIDTH / stride)

        for y in range(feature_map_height):
            for x in range(feature_map_width):
                for s in box_size:
                    x_center = (x + 0.5) * stride / IMG_WIDTH
                    y_center = (y + 0.5) * stride / IMG_HEIGHT
                    w = s / IMG_WIDTH
                    h = s / IMG_HEIGHT
                    anchors.append(Anchor(x_center, y_center, h, w))

    return anchors


class FaceDetector(object):
    def __init__(self):
        self.interpreter = MNN.Interpreter(util.get_resource(MODEL))
        self.session = self.interpreter.createSession()
        self.input_tensor = self.interpreter.getSessionInput(self.session)
        self.publisher = Publisher()
        self.mean = np.array([104.0, 117.0, 123.0], dtype="float32")
        self.norm = np.array([1.0, 1.0, 1.0], dtype="float32")
        self.anchors = gen_anchors()

    def __call__(self, topic, data):
        if topic == b"image":
            self.detect(data)

    def detect(self, image):
        # NOTE: image is BGR
        resized_image, mat = util.resize_and_keep_aspect_ratio(
            image, int(IMG_WIDTH), int(IMG_HEIGHT)
        )
        resized_image = resized_image.astype("float32")

        resized_image -= self.mean
        resized_image *= self.norm
        # NOTE: cv2 image format is (HWC), change it to (CHW)
        resized_image = resized_image.transpose((2, 0, 1))
        tmp_image = MNN.Tensor(
            (1, 3, IMG_HEIGHT, IMG_WIDTH),
            MNN.Halide_Type_Float,
            resized_image,
            MNN.Tensor_DimensionType_Caffe,
        )
        self.input_tensor.copyFrom(tmp_image)

        self.interpreter.runSession(self.session)
        raw_boxes, scores, raw_keypoints = [
            x.getData().squeeze()
            for x in self.interpreter.getSessionOutputAll(self.session).values()
        ]

        boxes = []
        for i in range(NUM_BOXES):
            if scores[i][1] < MIN_SCORE_THRESH:
                continue
            cx = (
                self.anchors[i].x_center + raw_boxes[i][0] * 0.1 * self.anchors[i].width
            )
            cy = (
                self.anchors[i].y_center
                + raw_boxes[i][1] * 0.1 * self.anchors[i].height
            )
            w = self.anchors[i].width * np.exp(raw_boxes[i][2] * 0.2)
            h = self.anchors[i].height * np.exp(raw_boxes[i][3] * 0.2)

            keypoints = []
            for j in range(NUM_KEYPOINTS):
                x = (
                    self.anchors[i].x_center
                    + raw_keypoints[i][j * 2] * 0.1 * self.anchors[i].width
                )
                y = (
                    self.anchors[i].y_center
                    + raw_keypoints[i][j * 2 + 1] * 0.1 * self.anchors[i].height
                )
                keypoints.append([x, y])

            boxes.append(Box(scores[i][1], (cx - w / 2, cy - h / 2, w, h), keypoints))

        if len(boxes) == 0:
            return

        boxes = NMS(boxes)
        # restore coordinates
        rect = np.array([IMG_WIDTH, IMG_HEIGHT])
        for box in boxes:
            # convert boxes to ndarray
            coord = [box.xmin, box.ymin, box.xmin + box.width, box.ymin + box.height]
            for point in box.keypoints:
                coord.append(point[0])
                coord.append(point[1])

            coord = np.array(coord).reshape(-1, 2)
            coord = util.restore_coords_2d(coord * rect, mat).astype("int")

            # convert ndarray back to boxes
            box.xmin = coord[0][0]
            box.ymin = coord[0][1]
            box.width = coord[1][0] - coord[0][0]
            box.height = coord[1][1] - coord[0][1]
            for i, point in enumerate(box.keypoints):
                point[0] = coord[2 + i][0]
                point[1] = coord[2 + i][1]

        # only on face is published
        self.publisher.pub(b"face_box", boxes[0])
