#!/usr/bin/env python3
# -*- coding: utf-8 -*-
import math
from cv2 import cv2
import numpy as np

from .config import *
from message_broker.transport import Publisher
from box_detection.box_detector import BoxDetector, BoxConfig

import util


class PalmDetector(object):
    def __init__(self):
        palm_box_config = BoxConfig(
            MODEL,
            IMG_HEIGHT,
            IMG_WIDTH,
            NUM_COORDS,
            NUM_BOXES,
            MIN_SCORE_THRESH,
            NMS_THRESH,
            NUM_KEYPOINT,
        )
        self.publisher = Publisher()
        self.box_detector = BoxDetector(palm_box_config)
        self.image = None
        self.box = None  # BoxDetector.Box

    def __call__(self, topic, data):
        if topic == b"image":
            self.image = data
            self.detect()

    def detect(self):
        boxes = self.box_detector.detect(self.image)
        if not boxes:
            # ZMQ_PUB: palm_reset
            self.publisher.pub(b"palm_reset")
            return
        # NEXT: only one palm is detected
        self.box = boxes[0]
        # ZMQ_PUB: palm_box
        self.publisher.pub(b"palm_box", self.box)
        # ZMQ_PUB: palm_roi
        self.publisher.pub(b"palm_roi", self.crop())

    def crop(self):
        # [[file:~/source/mediapipe/mediapipe/modules/hand_landmark/palm_detection_detection_to_roi.pbtxt::node {]]
        #
        #     1 2 3 4
        #  6
        #     5
        #        0
        x1 = self.box.xmin
        x2 = self.box.xmin + self.box.width
        y1 = self.box.ymin
        y2 = self.box.ymin + self.box.height

        shift_y = (y2 - y1) // 2
        margin_w, margin_h = (x2 - x1) * 4 // 5, (y2 - y1) * 4 // 5

        self.image = self.image[
            max(y1 - margin_h - shift_y, 0) : y2 + margin_h - shift_y,
            max(x1 - margin_w, 0) : x2 + margin_w,
        ]

        wrist_center, middle_finger = self.box.keypoints[0], self.box.keypoints[2]
        angle = (
            math.atan2(
                middle_finger[0] - wrist_center[0], wrist_center[1] - middle_finger[1]
            )
            * 57.3
        )
        rot_mat = cv2.getRotationMatrix2D(
            (self.image.shape[0] / 2, self.image.shape[1] / 2), angle, 1
        )
        image = cv2.warpAffine(
            self.image, rot_mat, (self.image.shape[1], self.image.shape[0])
        )
        translation_mat = util.get_translation_mat(
            x1 - margin_w, y1 - margin_h - shift_y
        )
        rotation_mat = cv2.getRotationMatrix2D(
            (image.shape[0] / 2, image.shape[1] / 2), -angle, 1
        )
        # convert to homogeneous coordinates
        rotation_mat = np.vstack([rotation_mat, [0, 0, 1]])
        return util.ROIImage(image, translation_mat @ rotation_mat)
