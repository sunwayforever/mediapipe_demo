#!/usr/bin/env python3
# -*- coding: utf-8 -*-
import math
from cv2 import cv2
import numpy as np
from collections import namedtuple

from .config import *
from message_broker.transport import Publisher
from box_detection.box_detector import BoxDetector, BoxConfig

import util

FaceROI = namedtuple("FaceROI", ["image", "mat"])


class FaceDetector(object):
    def __init__(self):
        face_box_config = BoxConfig(
            FACE_MODEL,
            IMG_HEIGHT,
            IMG_WIDTH,
            NUM_COORDS,
            NUM_BOXES,
            MIN_SCORE_THRESH,
            NMS_THRESH,
            NUM_KEYPOINT,
        )
        self.publisher = Publisher()
        self.box_detector = BoxDetector(face_box_config)
        self.image = None
        self.box = None  # type BoxDetector.Box

    def __call__(self, topic, data):
        if topic == b"image":
            self.image = data
            self.detect()

    def detect(self):
        boxes = self.box_detector.detect(self.image)
        if not boxes:
            # ZMQ_PUB: face_reset
            self.publisher.pub(b"face_reset")
            return
        # NEXT: only one face is detected
        self.box = boxes[0]
        # ZMQ_PUB: face_box
        self.publisher.pub(b"face_box", self.box)
        face = self.crop()
        # ZMQ_PUB: face_roi
        self.publisher.pub(b"face_roi", face)
        # ZMQ_PUB: face_roi_slow
        self.publisher.pub(b"face_roi_slow", face)

    def crop(self):
        x1 = self.box.xmin
        x2 = self.box.xmin + self.box.width
        y1 = self.box.ymin
        y2 = self.box.ymin + self.box.height

        # [[file:~/source/mediapipe/mediapipe/modules/face_landmark/face_detection_front_detection_to_roi.pbtxt::\[mediapipe.RectTransformationCalculatorOptions.ext\] {]]
        margin_w, margin_h = (x2 - x1) // 4, (y2 - y1) // 4

        self.image = self.image[
            max(y1 - margin_h, 0) : y2 + margin_h, max(x1 - margin_w, 0) : x2 + margin_w
        ]

        left_eye, right_eye = self.box.keypoints[0], self.box.keypoints[1]
        angle = (
            math.atan2(right_eye[1] - left_eye[1], right_eye[0] - left_eye[0]) * 57.3
        )
        rot_mat = cv2.getRotationMatrix2D(
            (self.image.shape[0] / 2, self.image.shape[1] / 2), angle, 1
        )
        self.image = cv2.warpAffine(
            self.image, rot_mat, (self.image.shape[1], self.image.shape[0])
        )

        translation_mat = util.get_translation_mat(x1 - margin_w, y1 - margin_h)
        rotation_mat = cv2.getRotationMatrix2D(
            (self.image.shape[0] / 2, self.image.shape[1] / 2), -angle, 1
        )
        # convert to homogeneous coordinates
        rotation_mat = np.vstack([rotation_mat, [0, 0, 1]])
        return FaceROI(self.image, translation_mat @ rotation_mat)
