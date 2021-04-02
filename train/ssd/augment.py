#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# 2021-04-02 16:07
import numpy as np


class Augment(object):
    @staticmethod
    def original(img, boxes, labels):
        return img, boxes, labels

    @staticmethod
    def flip(img, boxes, labels):
        img = img[:, ::-1, :]
        h, w = img.shape[:2]
        # box: [n,4] x1,y1,x2,y2
        boxes = np.stack(
            [w - boxes[:, 2], boxes[:, 1], w - boxes[:, 0], boxes[:, 3]], axis=1
        )
        return img.copy(), boxes, labels

    def __init__(self):
        self.choices = [Augment.original] * 5 + [Augment.flip] * 3

    def __call__(self, img, boxes, labels):
        f = np.random.choice(self.choices)
        return f(img, boxes, labels)


if __name__ == "__main__":
    import cv2
    from voc_dataset import VOCDataset

    dataset = VOCDataset("/home/sunway/download/VOCdevkit/VOC2007")
    img = dataset._get_image("009126")
    boxes, labels = dataset._get_annotation("009126")

    for box in boxes:
        cv2.rectangle(img, tuple(box[:2]), tuple(box[2:]), (255, 0, 0), 1, 1)
    cv2.imshow("orign", img)

    img, boxes, labels = Augment.flip(img, boxes, labels)
    for box in boxes:
        cv2.rectangle(img, tuple(box[:2]), tuple(box[2:]), (255, 0, 0), 1, 1)
    cv2.imshow("flipped", img)
    cv2.waitKey(0)
