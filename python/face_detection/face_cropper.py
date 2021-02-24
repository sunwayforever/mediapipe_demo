#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# 2020-12-15 21:04
import math
import cv2
import numpy as np
from collections import namedtuple
import util

FaceROI = namedtuple("FaceROI", ["image", "mat"])


class FaceCropper(object):
    def __init__(self):
        pass

    def crop(self, img, box):
        img_height = img.shape[0]
        img_width = img.shape[1]

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
            math.atan((right_eye[1] - left_eye[1]) / (right_eye[0] - left_eye[0]))
            * 57.3
        )
        rot_mat = cv2.getRotationMatrix2D(
            (img.shape[0] / 2, img.shape[1] / 2), angle, 1
        )
        img = cv2.warpAffine(img, rot_mat, (img.shape[1], img.shape[0]))

        translation_mat = util.get_translation_mat(x1 - margin_w, y1 - margin_h)
        rotation_mat = cv2.getRotationMatrix2D(
            (img.shape[0] / 2, img.shape[1] / 2), -angle, 1
        )
        # convert to homogeneous coordinates
        rotation_mat = np.vstack([rotation_mat, [0, 0, 1]])
        return FaceROI(img, translation_mat @ rotation_mat)
