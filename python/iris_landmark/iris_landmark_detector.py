#!/usr/bin/env python3
# -*- coding: utf-8 -*-
from cv2 import cv2
import numpy as np
import tensorflow as tf
from tensorflow import keras
import onnxruntime as onnx

from .config import *
from .iris_points import *
from .eye_estimator import EyeEstimator
from message_broker.transport import Publisher
from common import util
from common.detector import Detector


class IrisLandmarkDetector(Detector):
    def __init__(self):
        super().__init__(
            util.get_resource(MODEL),
            {
                "tflite": [0, 1],
                "onnx": ["output_eyes_contours_and_brows", "output_iris"],
                "tf": ["output_0", "output_1"],
            },
        )
        self.publisher = Publisher()
        self.eye_estimator = EyeEstimator()

    def __call__(self, topic, data):
        if topic == b"iris_roi":
            self.detect(data)

    def detect(self, iris_roi):
        eye_surfaces = []
        iris_surfaces = []
        for index, (img, mat) in enumerate(iris_roi):
            # right eye need to be horizontally flipped
            if index == 1:
                img = cv2.flip(img, 1)

            scale_mat = util.get_scale_mat(
                img.shape[1] / IMG_WIDTH,
                img.shape[0] / IMG_HEIGHT,
            )
            mat = mat @ scale_mat

            input_data = cv2.resize(img, (IMG_WIDTH, IMG_HEIGHT)).astype(np.float32)
            input_data = input_data / 255.0
            input_data = np.expand_dims(input_data, axis=0)

            eye_surface, iris_surface = super().invoke(input_data)

            eye_surface, iris_surface = (
                np.reshape(eye_surface, (-1, 3)).copy(),
                np.reshape(iris_surface, (-1, 3)).copy(),
            )

            # right eye was horizontally flipped
            if index == 1:
                eye_surface[:, 0] = IMG_WIDTH - eye_surface[:, 0]
                iris_surface[:, 0] = IMG_WIDTH - iris_surface[:, 0]

            eye_surface = util.restore_coordinates(eye_surface, mat)
            iris_surface = util.restore_coordinates(iris_surface, mat)

            eye_surfaces.append(eye_surface.astype(int))
            iris_surfaces.append(iris_surface.astype(int))

        # ZMQ_PUB: eye_landmark
        self.publisher.pub(b"eye_landmark", eye_surfaces)
        # ZMQ_PUB: iris_landmark
        self.publisher.pub(b"iris_landmark", iris_surfaces)
        # ZMQ_PUB: eye_aspect_ratio
        self.publisher.pub(
            b"eye_aspect_ratio",
            self.eye_estimator.estimate(
                [surface[eye_points] for surface in eye_surfaces]
            ),
        )
