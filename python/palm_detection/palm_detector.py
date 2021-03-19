#!/usr/bin/env python3
# -*- coding: utf-8 -*-
import math
import cv2
import numpy as np

from .config import *
from message_broker.transport import Publisher
from box_detection.box_detector import BoxDetector, BoxConfig

from common import util, PointVelocityFilter


class PalmDetector(object):
    def __init__(self):
        palm_box_config = BoxConfig(
            util.get_resource(MODEL),
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
        self.point_velocity_filters = [
            PointVelocityFilter(cov_measure=0.001) for _ in range(2)
        ]

    def __call__(self, topic, data):
        if topic == b"image":
            self.detect(data)

    def detect(self, image):
        boxes = self.box_detector.detect(image)
        if not boxes:
            # ZMQ_PUB: palm_reset
            self.publisher.pub(b"palm_reset")
            return
        # NEXT: only one palm is detected
        box = boxes[0]
        # filter
        box.xmin, box.ymin = (
            self.point_velocity_filters[0]
            .update(np.array([[box.xmin], [box.ymin]]).astype(np.float32))
            .astype(np.int)
        )
        box.width, box.height = (
            self.point_velocity_filters[1]
            .update(np.array([[box.width], [box.height]]).astype(np.float32))
            .astype(np.int)
        )
        # ZMQ_PUB: palm_box
        self.publisher.pub(b"palm_box", box)
        # ZMQ_PUB: palm_roi
        self.publisher.pub(b"palm_roi", self.crop(image, box))

    @staticmethod
    def crop(image, box):
        # [[file:~/source/mediapipe/mediapipe/modules/hand_landmark/palm_detection_detection_to_roi.pbtxt::node {]]
        #
        #     1 2 3 4
        #  6
        #     5
        #        0
        x1 = box.xmin
        x2 = box.xmin + box.width
        y1 = box.ymin
        y2 = box.ymin + box.height

        x1, y1, x2, y2 = util.square_rect(x1, y1, x2, y2)

        shift_y = (y2 - y1) // 2
        margin_w, margin_h = (x2 - x1) * 4 // 5, (y2 - y1) * 4 // 5

        image = image[
            max(y1 - margin_h - shift_y, 0) : y2 + margin_h - shift_y,
            max(x1 - margin_w, 0) : x2 + margin_w,
        ]

        wrist_center, middle_finger = box.keypoints[0], box.keypoints[2]
        angle = (
            math.atan2(
                middle_finger[0] - wrist_center[0], wrist_center[1] - middle_finger[1]
            )
            * 57.3
        )
        rot_mat = cv2.getRotationMatrix2D(
            (image.shape[0] / 2, image.shape[1] / 2), angle, 1
        )
        image = cv2.warpAffine(image, rot_mat, (image.shape[1], image.shape[0]))
        translation_mat = util.get_translation_mat(
            x1 - margin_w, y1 - margin_h - shift_y
        )
        rotation_mat = cv2.getRotationMatrix2D(
            (image.shape[0] / 2, image.shape[1] / 2), -angle, 1
        )
        # convert to homogeneous coordinates
        rotation_mat = np.vstack([rotation_mat, [0, 0, 1]])
        return util.ROIImage(image, translation_mat @ rotation_mat)
