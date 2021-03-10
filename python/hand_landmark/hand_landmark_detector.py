#!/usr/bin/env python3
# -*- coding: utf-8 -*-
from cv2 import cv2
import numpy as np

from .config import *
from .hand_points import *
from .hand_gesture_estimator import HandGestureEstimator
from message_broker.transport import Publisher
from common import util, Detector, PointVelocityFilter


class HandLandmarkDetector(Detector):
    def __init__(self):
        super().__init__(
            util.get_resource(MODEL),
            {
                "tflite": [0, 1],
                "onnx": ["Identity", "Identity_1"],
                "tf": ["output_0", "output_1"],
            },
        )
        self.publisher = Publisher()
        self.gesture_estimator = HandGestureEstimator()
        self.point_velocity_filters = [
            PointVelocityFilter() for _ in range(N_HAND_POINTS)
        ]

    def __call__(self, topic, data):
        if topic == b"palm_roi":
            self.detect(data)

    def detect(self, palm_roi):
        palm_img, mat = palm_roi.image, palm_roi.mat
        scale_mat = util.get_scale_mat(
            palm_img.shape[1] / IMG_WIDTH,
            palm_img.shape[0] / IMG_HEIGHT,
        )
        mat = mat @ scale_mat

        palm_img = cv2.resize(palm_img, (IMG_WIDTH, IMG_HEIGHT))
        input_data = palm_img.astype(np.float32) / 255.0
        input_data = np.expand_dims(input_data, axis=0)

        surface, prob = super().invoke(input_data)

        surface, prob = np.reshape(surface, (-1, 3)), np.squeeze(prob)
        surface = np.array(surface)
        if prob < MIN_PROB_THRESH:
            return None
        surface = util.restore_coordinates(surface, mat)
        surface = surface.astype("float32")
        # filter
        for filter, point in zip(self.point_velocity_filters, surface):
            point[:2] = filter.update(point[:2])

        # ZMQ_PUB: hand_landmark
        self.publisher.pub(b"hand_landmark", surface)
        # ZMQ_PUB: hand_gesture
        gesture = self.gesture_estimator.estimate(surface)
        self.publisher.pub(b"hand_gesture", gesture)
