#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# 2021-04-02 16:07
import cv2
import numpy as np
import tensorflow as tf

from config import *


class Transform(object):
    @staticmethod
    def Resize(image, boxes, labels):
        return cv2.resize(image, (IMAGE_SIZE, IMAGE_SIZE)), boxes, labels

    @staticmethod
    def ImagenetPreprocessing(image, boxes, labels):
        image = (image - 127.5) / 127.5
        return image, boxes, labels

    @staticmethod
    def RandomFlip(image, boxes, labels):
        if np.random.randint(2):
            return image, boxes, labels
        image = image[:, ::-1, :]
        # box: [n,4] x1,y1,x2,y2
        boxes = np.stack(
            [1 - boxes[:, 2], boxes[:, 1], 1 - boxes[:, 0], boxes[:, 3]], axis=1
        )
        return image.copy(), boxes, labels

    @staticmethod
    def RandomHSV(image, boxes, labels):
        image = cv2.cvtColor(image, cv2.COLOR_RGB2HSV).astype("float32")
        # Hue
        if np.random.randint(2):
            image[:, :, 0] += np.random.uniform(-36, 36)
            image[:, :, 0][image[:, :, 0] > 360.0] -= 360.0
            image[:, :, 0][image[:, :, 0] < 0.0] += 360.0
        # Saturation
        if np.random.randint(2):
            delta = np.random.uniform(0.5, 1.5)
            image[:, :, 1] *= delta
        image = cv2.cvtColor(image.astype("uint8"), cv2.COLOR_HSV2RGB)
        return image.copy(), boxes, labels

    @staticmethod
    def RandomContrast(image, boxes, labels):
        if np.random.randint(2):
            image = image.astype("float32")
            alpha = np.random.uniform(0.8, 1.2)
            image *= alpha
        return image.astype("uint8"), boxes, labels

    @staticmethod
    def RandomBrightness(image, boxes, labels):
        # if np.random.randint(2):
        image = image.astype("int")
        delta = np.random.randint(-10, 10)
        image += delta
        image = np.clip(image, 0, 255).astype("uint8")
        return image, boxes, labels

    def __init__(self):
        self.transforms = [
            # Transform.RandomBrightness,
            # Transform.RandomContrast,
            Transform.RandomHSV,
            Transform.RandomFlip,
            Transform.ImagenetPreprocessing,
            Transform.Resize,
        ]

    def __call__(self, image, boxes, labels):
        for f in self.transforms:
            image, boxes, labels = f(image, boxes, labels)
        return image, boxes, labels


if __name__ == "__main__":
    import cv2
    from voc_dataset import VOCDataset

    voc = VOCDataset("/home/sunway/download/VOCdevkit/VOC2007")
    image = voc._get_image("009126")
    boxes, labels = voc._get_annotation("009126")

    for box in boxes:
        cv2.rectangle(image, tuple(box[:2]), tuple(box[2:]), (255, 0, 0), 1, 1)
    cv2.imshow("orign", image)
    transform = Transform()
    image, boxes, labels = transform(image, boxes, labels)
    for box in boxes:
        cv2.rectangle(image, tuple(box[:2]), tuple(box[2:]), (255, 0, 0), 1, 1)
    cv2.imshow("transformed", image)
    cv2.waitKey(0)
