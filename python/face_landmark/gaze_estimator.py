#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# 2021-03-17 13:24
import numpy as np
from cv2 import cv2

from .config import *
from common import util, Detector


class GazeEstimator(Detector):
    def __init__(self):
        super().__init__(util.get_resource(GAZE_MODEL), {"onnx": ["156"]})
        self.camera_matrix = np.linalg.inv(
            np.array([[640, 0, 320], [0, 640, 240], [0, 0, 1]], dtype=np.float32)
        )
        self.normalized_camera_matrix = np.array(
            [[1600, 0, 112], [0, 1600, 112], [0, 0, 1]], dtype=np.float32
        )

    def estimate(self, image, distance, rot):
        image = self.get_normalized_image(image, distance, rot)
        image = np.expand_dims(image, axis=0)
        data = super().invoke(image)[0]
        pitch, yaw = data[0]
        normalized_gaze_vector = -np.array(
            [np.cos(pitch) * np.sin(yaw), np.sin(pitch), np.cos(pitch) * np.cos(yaw)]
        )
        gaze_vector = normalized_gaze_vector @ rot.as_matrix()
        print(np.rad2deg(GazeEstimator.vector_to_angle(gaze_vector)))

    @staticmethod
    def vector_to_angle(vector: np.ndarray) -> np.ndarray:
        assert vector.shape == (3,)
        x, y, z = vector
        pitch = np.arcsin(-y)
        yaw = np.arctan2(-x, -z)
        return np.array([pitch, yaw])

    def _get_scale_matrix(self, distance):
        return np.array(
            [
                [1, 0, 0],
                [0, 1, 0],
                [0, 0, 1.0 / distance],
            ],
            dtype=np.float,
        )

    def get_normalized_image(self, image, distance, rot):
        scale = self._get_scale_matrix(distance)
        conversion_matrix = scale @ rot.as_matrix()
        projection_matrix = (
            self.normalized_camera_matrix @ conversion_matrix @ self.camera_matrix
        )
        image = cv2.warpPerspective(
            image,
            projection_matrix,
            (224, 224),
        )
        image = image.astype(np.float32) / 255.0
        return image
