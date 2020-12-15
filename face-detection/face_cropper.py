#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# 2020-12-15 21:04
from face_detector import Detector


class FaceCropper(object):
    def __init__(self):
        self.detector = Detector()

    def __call__(self, img):
        boxes = self.detector(img)
        img = crop_image(img, boxes)
        return img


def crop_image(img, boxes):
    img_height = img.shape[0]
    img_width = img.shape[1]

    if len(boxes) == 0:
        return None

    box = boxes[0]
    x1 = int(img_width * box.xmin)
    x2 = int(img_width * (box.xmin + box.width))
    y1 = int(img_height * box.ymin)
    y2 = int(img_height * (box.ymin + box.height))

    margin_w, margin_h = max((x2 - x1) // 4, 0), max((y2 - y1) // 4, 0)

    return (
        img[y1 - margin_h : y2 + margin_h, x1 - margin_w : x2 + margin_w],
        x1 - margin_w,
        y1 - margin_h,
    )
