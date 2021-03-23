#!/usr/bin/env python3
# -*- coding: utf-8 -*-
import cv2
import numpy as np

from .config import *
from .face_points import *
from .pose_estimator import PoseEstimator
from .mouth_estimator import MouthEstimator
from .iris_cropper import IrisCropper
from message_broker import Publisher
from common import util, Detector, PointVelocityFilter


class FaceLandmarkDetector(Detector):
    def __init__(self):
        super().__init__(
            util.get_resource(MODEL),
            {
                "tflite": [0, 1],
                "onnx": ["conv2d_21", "conv2d_31"],
                "tf": ["output_0", "output_1"],
            },
        )
        self.publisher = Publisher()
        self.pose_estimator = PoseEstimator()
        self.mouth_estimator = MouthEstimator()
        self.iris_cropper = IrisCropper()
        self.point_velocity_filters = [
            PointVelocityFilter() for _ in range(N_FACE_POINTS)
        ]

    def __call__(self, topic, data):
        if topic == b"face_roi":
            self.detect(data)

    def detect(self, face_roi):
        orig_face_img, mat = face_roi.image, face_roi.mat
        scale_mat = util.get_scale_mat(
            orig_face_img.shape[1] / IMG_WIDTH,
            orig_face_img.shape[0] / IMG_HEIGHT,
        )
        mat = mat @ scale_mat

        face_img = cv2.resize(orig_face_img, (IMG_WIDTH, IMG_HEIGHT))
        input_data = face_img.astype(np.float32) / 255.0
        input_data = np.expand_dims(input_data, axis=0)

        surface, prob = super().invoke(input_data)

        surface, prob = np.reshape(surface, (-1, 3)), np.squeeze(prob)
        surface = np.array(surface)

        if util.sigmoid(prob) < MIN_PROB_THRESH:
            return None

        # ZMQ_PUB: iris_roi
        self.publisher.pub(b"iris_roi", self.iris_cropper.crop(face_img, surface, mat))

        surface = util.restore_coordinates_3d(surface, mat).astype("float32")
        # filter
        for filter, point in zip(self.point_velocity_filters, surface):
            point[:2] = filter.update(point[:2])

        # ZMQ_PUB: face_landmark
        self.publisher.pub(b"face_landmark", surface)
        # # ZMQ_PUB: rotation
        self.publisher.pub(
            b"rotation", self.pose_estimator.estimate(surface[pose_points, :2])
        )
        # ZMQ_PUB: mouth_aspect_ratio
        self.publisher.pub(
            b"mouth_aspect_ratio",
            self.mouth_estimator.estimate(
                surface[
                    mouth_points,
                ]
            ),
        )
