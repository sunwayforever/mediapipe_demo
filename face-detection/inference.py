#!/usr/bin/env python3
# -*- coding: utf-8 -*-
import cv2
import time
import math
import numpy as np
import tensorflow as tf
import matplotlib.pyplot as plt

IMG_HEIGHT, IMG_WIDTH = 128, 128
NUM_COORDS = 16
NUM_BOXES = 896
MIN_SCORE_THRESH = 0.75
NMS_THRESH = 0.85


class Anchor:
    def __init__(self, x_center, y_center, h, w):
        self.x_center = x_center
        self.y_center = y_center
        self.h = h
        self.w = w


class Box:
    def __init__(self, score, xmin, ymin, width, height):
        self.score = score
        self.xmin = xmin
        self.ymin = ymin
        self.width = width
        self.height = height


def calibrate(raw_boxes, i, anchors):
    box_data = np.zeros(4)
    box_offset = i * NUM_COORDS

    # x_center,y_center 是 box 中心坐标距其对应的 anchor 的绝对偏移量
    # w,h 是 box 的绝对大小
    x_center = raw_boxes[box_offset]
    y_center = raw_boxes[box_offset + 1]
    w = raw_boxes[box_offset + 2]
    h = raw_boxes[box_offset + 3]

    # x_center, y_center 变为 box 的绝对中心坐标 (0~1)
    x_center = x_center / IMG_WIDTH + anchors[i].x_center
    y_center = y_center / IMG_HEIGHT + anchors[i].y_center

    # h, w 变为 box 的绝对大小 (0~1)
    h = h / IMG_HEIGHT
    w = w / IMG_WIDTH

    box_data[0] = x_center - w / 2.0
    box_data[1] = y_center - h / 2.0
    box_data[2] = w
    box_data[3] = h

    return box_data


def detect(raw_boxes, anchors_, detection_scores):
    detection_scores = sigmoid(np.clip(detection_scores, -100, 100))
    boxes = []
    for i in range(NUM_BOXES):
        if detection_scores[i] < MIN_SCORE_THRESH:
            continue

        box = calibrate(raw_boxes, i, anchors_)
        box = Box(
            detection_scores[i],
            box[0],
            box[1],
            box[2],
            box[3],
        )
        boxes.append(box)
    return boxes


def sigmoid(x):
    return 1.0 / (1.0 + np.exp(-x))


def NMS(boxes, threshold):
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
        # 计算 box 中所有其它 box (I[0:-1]) 与 best (I[-1]) 的 IOU过滤掉 IOU >=
        # NMS_THRESH 的 box, 因为它们与当前 best 有很大的重叠
        xx1 = np.maximum(x1[best], x1[I[0:-1]])
        yy1 = np.maximum(y1[best], y1[I[0:-1]])
        xx2 = np.minimum(x2[best], x2[I[0:-1]])
        yy2 = np.minimum(y2[best], y2[I[0:-1]])
        w = np.maximum(0.0, xx2 - xx1 + 1)
        h = np.maximum(0.0, yy2 - yy1 + 1)
        intersection = w * h
        iou = intersection / (area[best] + area[I[0:-1]] - intersection)
        I = I[np.where(iou <= threshold)[0]]
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


def main():
    model_path = "model.tflite"

    # Load TFLite model and allocate tensors.
    interpreter = tf.lite.Interpreter(model_path=model_path)
    interpreter.allocate_tensors()
    # Get input and output tensors.
    input_details = interpreter.get_input_details()
    output_details = interpreter.get_output_details()

    # print('--------------------------------')
    # print("input_details: ")
    # print(input_details)
    # print("output_details: ")
    # print(output_details)

    img = cv2.imread("image/1.png")
    img_height = img.shape[0]
    img_width = img.shape[1]

    img_rgb = cv2.cvtColor(img, cv2.COLOR_BGR2RGB)

    input_width = input_details[0]["shape"][1]
    input_height = input_details[0]["shape"][2]
    input_data = cv2.resize(img_rgb, (input_width, input_height)).astype(np.float32)
    input_data = (input_data - 127.5) / 127.5
    input_data = np.expand_dims(input_data, axis=0)

    interpreter.set_tensor(input_details[0]["index"], input_data)
    interpreter.invoke()
    regressors = interpreter.get_tensor(output_details[0]["index"])
    classificators = interpreter.get_tensor(output_details[1]["index"])

    raw_boxes = np.reshape(regressors, (-1))
    raw_scores = np.reshape(classificators, (-1))

    anchors = gen_anchors()
    detections = detect(raw_boxes, anchors, raw_scores)
    detections = NMS(detections, NMS_THRESH)

    for detection in detections:
        x1 = int(img_width * detection.xmin)
        x2 = int(img_width * (detection.xmin + detection.width))
        y1 = int(img_height * detection.ymin)
        y2 = int(img_height * (detection.ymin + detection.height))

        cv2.rectangle(img, (x1, y1), (x2, y2), (255, 0, 0), 2)
        cv2.putText(
            img,
            "{:.2f}".format(detection.score),
            (x1, y1 - 6),
            cv2.FONT_HERSHEY_SIMPLEX,
            0.6,
            (0, 0, 255),
            2,
        )

    cv2.imshow("", img)
    cv2.waitKey(0)
    cv2.destroyAllWindows()


if __name__ == "__main__":
    main()
