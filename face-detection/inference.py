#!/usr/bin/env python3
# -*- coding: utf-8 -*-
import cv2
import time
import math
import argparse
import numpy as np
import tensorflow as tf
import matplotlib.pyplot as plt
import time

IMG_HEIGHT, IMG_WIDTH = 128, 128
NUM_COORDS = 16
NUM_BOXES = 896
MIN_SCORE_THRESH = 0.5
NMS_THRESH = 0.85
NUM_KEYPOINT = 6


class Anchor:
    def __init__(self, x_center, y_center, h, w):
        self.x_center = x_center
        self.y_center = y_center
        self.h = h
        self.w = w


class Box:
    def __init__(self, score, box):
        self.score = score
        self.xmin = box[0]
        self.ymin = box[1]
        self.width = box[2]
        self.height = box[3]
        self.keypoints = []
        for i in range(NUM_KEYPOINT):
            self.keypoints.append((box[4 + 2 * i], box[4 + 2 * i + 1]))


def calibrate(raw_boxes, index, anchors):
    box_data = np.zeros(NUM_COORDS)
    box_offset = index * NUM_COORDS

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
    x_center = x_center / IMG_WIDTH * anchors[index].w + anchors[index].x_center
    y_center = y_center / IMG_HEIGHT * anchors[index].h + anchors[index].y_center

    # h, w 变为 box 的绝对大小 (0~1)
    h = h / IMG_HEIGHT * anchors[index].h
    w = w / IMG_WIDTH * anchors[index].w

    box_data[0] = x_center - w / 2.0
    box_data[1] = y_center - h / 2.0
    box_data[2] = w
    box_data[3] = h

    # for 6 keypoint
    for j in range(NUM_KEYPOINT * 2):
        box_data[4 + j] = raw_boxes[box_offset + 4 + j]
        if j % 2 == 0:
            box_data[4 + j] = box_data[4 + j] / IMG_WIDTH + anchors[index].x_center
        else:
            box_data[4 + j] = box_data[4 + j] / IMG_HEIGHT + anchors[index].y_center

    return box_data


def detect(raw_boxes, anchors_, detection_scores):
    detection_scores = sigmoid(np.clip(detection_scores, -100, 100))
    boxes = []
    for i in range(NUM_BOXES):
        if detection_scores[i] < MIN_SCORE_THRESH:
            continue

        box = calibrate(raw_boxes, i, anchors_)
        box = Box(detection_scores[i], box)
        boxes.append(box)
    return boxes


def sigmoid(x):
    return 1.0 / (1.0 + np.exp(-x))


def NMS(boxes):
    # Not Max Supression
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
        # 计算 box 中所有其它 box (I[0:-1]) 与 best (I[-1]) 的 IOU, 过滤掉 IOU
        # >= NMS_THRESH 的 box, 因为它们与当前 best 有很大的重叠
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


def gen_anchors():
    anchors = []
    for stride, count in zip([8, 16], [1, 3]):
        feature_map_height = math.ceil(1.0 * IMG_HEIGHT / stride)
        feature_map_width = math.ceil(1.0 * IMG_WIDTH / stride)

        for y in range(feature_map_height):
            for x in range(feature_map_width):
                for anchor_id in range(2 * count):
                    x_center = (x + 0.5) / feature_map_width
                    y_center = (y + 0.5) / feature_map_height
                    w = 1.0
                    h = 1.0
                    new_anchor = Anchor(x_center, y_center, h, w)
                    anchors.append(new_anchor)

    return anchors


class Inference(object):
    def __init__(self):
        model_path = "front.tflite"
        # Load TFLite model and allocate tensors.
        self.interpreter = tf.lite.Interpreter(model_path=model_path)
        self.interpreter.allocate_tensors()
        self.input_details = self.interpreter.get_input_details()
        self.output_details = self.interpreter.get_output_details()

        self.input_width = self.input_details[0]["shape"][1]
        self.input_height = self.input_details[0]["shape"][2]
        assert self.input_width == IMG_WIDTH
        assert self.input_height == IMG_HEIGHT

    def __call__(self, img):
        img_rgb = cv2.cvtColor(img, cv2.COLOR_BGR2RGB)

        input_data = cv2.resize(img_rgb, (self.input_width, self.input_height)).astype(
            np.float32
        )
        input_data = (input_data - 127.5) / 127.5
        input_data = np.expand_dims(input_data, axis=0)

        self.interpreter.set_tensor(self.input_details[0]["index"], input_data)
        self.interpreter.invoke()

        regressors = self.interpreter.get_tensor(self.output_details[0]["index"])
        classificators = self.interpreter.get_tensor(self.output_details[1]["index"])

        raw_boxes = np.reshape(regressors, (-1))
        raw_scores = np.reshape(classificators, (-1))

        return NMS(detect(raw_boxes, gen_anchors(), raw_scores))


def annotate_image(img, boxes):
    img_height = img.shape[0]
    img_width = img.shape[1]

    for box in boxes:
        x1 = int(img_width * box.xmin)
        x2 = int(img_width * (box.xmin + box.width))
        y1 = int(img_height * box.ymin)
        y2 = int(img_height * (box.ymin + box.height))

        cv2.rectangle(img, (x1, y1), (x2, y2), (255, 0, 0), 2)
        cv2.putText(
            img,
            "{:.2f}".format(box.score),
            (x1, y1 - 6),
            cv2.FONT_HERSHEY_SIMPLEX,
            0.6,
            (0, 0, 255),
            2,
        )

        for point in box.keypoints:
            cv2.circle(
                img,
                (int(img_width * point[0]), int(img_height * point[1])),
                color=(0, 255, 0),
                radius=3,
                thickness=3,
            )
    return img


def inference_image(img):
    inference = Inference()
    img = cv2.imread(img)
    boxes = inference(img)
    img = annotate_image(img, boxes)
    cv2.imshow("", img)
    cv2.waitKey(0)
    cv2.destroyAllWindows()


def inference_stream():
    inference = Inference()
    vid = cv2.VideoCapture(0)
    while True:
        _, img = vid.read()
        boxes = inference(img)
        img = annotate_image(img, boxes)
        cv2.imshow("", img)
        if cv2.waitKey(1) & 0xFF == ord("q"):
            break

    vid.release()
    cv2.destroyAllWindows()


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    group = parser.add_mutually_exclusive_group(required=True)
    group.add_argument("--stream", action="store_true")
    group.add_argument("--image", type=str)
    flags = parser.parse_args()
    if flags.image:
        inference_image(flags.image)
    else:
        inference_stream()
