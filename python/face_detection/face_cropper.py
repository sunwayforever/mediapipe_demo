#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# 2020-12-15 21:04
import math
import cv2

from .face_detector import Detector

class FaceCropper(object):
    def __init__(self):
        self.boxes = None
        self.img = None
        self.detector = Detector()

    @staticmethod
    def _distance(a, b):
        if a is None or b is None:
            return 100000
        if a.shape != b.shape:
            a = cv2.resize(a, (256, 256))
            b = cv2.resize(b, (256, 256))
        return cv2.norm(a, b)

    def _crop_image(self, img, tracking):
        if self.boxes is None or not tracking:
            print("lose track")
            self.boxes = self.detector(img)
        ret = crop_image(img, self.boxes)
        if self.img is None or not tracking:
            self.img = ret[0]
        return ret

    def __call__(self, img):
        ret = self._crop_image(img, True)
        distance = FaceCropper._distance(ret[0], self.img)
        if distance > 5000:
            ret = self._crop_image(img, False)
        return ret


def crop_image(img, boxes):
    img_height = img.shape[0]
    img_width = img.shape[1]

    if len(boxes) == 0:
        return None, 0, 0, 0

    # single face only
    box = boxes[0]
    x1 = int(img_width * box.xmin)
    x2 = int(img_width * (box.xmin + box.width))
    y1 = int(img_height * box.ymin)
    y2 = int(img_height * (box.ymin + box.height))

    # [[file:~/source/mediapipe/mediapipe/modules/face_landmark/face_detection_front_detection_to_roi.pbtxt::\[mediapipe.RectTransformationCalculatorOptions.ext\] {]]
    margin_w, margin_h = (x2 - x1) // 4, (y2 - y1) // 4

    img = img[
        max(y1 - margin_h, 0) : y2 + margin_h, max(x1 - margin_w, 0) : x2 + margin_w
    ]

    left_eye, right_eye = box.keypoints[0], box.keypoints[1]
    angle = (
        math.atan((right_eye[1] - left_eye[1]) / (right_eye[0] - left_eye[0])) * 57.3
    )
    rot_mat = cv2.getRotationMatrix2D((img.shape[0] / 2, img.shape[1] / 2), angle, 1)
    img = cv2.warpAffine(img, rot_mat, (img.shape[1], img.shape[0]))
    return (
        img,
        x1 - margin_w,
        y1 - margin_h,
        cv2.getRotationMatrix2D((img.shape[0] / 2, img.shape[1] / 2), -angle, 1),
    )


if __name__ == "__main__":
    import argparse
    import numpy as np

    parser = argparse.ArgumentParser()
    parser.add_argument("file_name", type=str)
    flags = parser.parse_args()

    cropper = FaceCropper()
    img = cv2.imread(flags.file_name)
    img, _, _, _ = cropper(img)
    cv2.imshow("", img)
    while cv2.waitKey(1) & 0xFF != ord("q"):
        pass
