#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# 2021-03-17 16:30
import numpy as np
from cv2 import cv2
from scipy.spatial.transform import Rotation

from message_broker.transport import Publisher
from common import util, Detector


class GazeEstimator(Detector):
    def __init__(self):
        super().__init__(util.get_resource("../model/gaze.onnx"), {"onnx": ["156"]})
        self.publisher = Publisher()
        self.image = None
        self.REYE_INDICES = np.array([36, 39])
        self.LEYE_INDICES = np.array([42, 45])
        self.MOUTH_INDICES = np.array([48, 54])

        # fmt: off
        self.landmark_index = np.array([
            127, 227, 137, 213, 138, 136, 149, 171, 152, 400,
            378, 394, 397, 435, 401, 454, 356, 70, 63, 105,
            66, 55, 285, 336, 296, 334, 300, 168, 197, 195,
            4, 60, 2, 164, 325, 337, 153, 159, 157, 112,
            154, 145, 381, 385, 387, 249, 373, 374, 146, 73,
            0, 302, 267, 304, 375, 405, 314, 17, 84, 181,
            90, 86, 15, 318, 321, 319, 16, 72,
        ])
        # fmt: on
        self.LANDMARKS = np.array(
            [
                [-0.07141807, -0.02827123, 0.08114384],
                [-0.07067417, -0.00961522, 0.08035654],
                [-0.06844646, 0.00895837, 0.08046731],
                [-0.06474301, 0.02708319, 0.08045689],
                [-0.05778475, 0.04384917, 0.07802191],
                [-0.04673809, 0.05812865, 0.07192291],
                [-0.03293922, 0.06962711, 0.06106274],
                [-0.01744018, 0.07850638, 0.04752971],
                [0.0, 0.08105961, 0.0425195],
                [0.01744018, 0.07850638, 0.04752971],
                [0.03293922, 0.06962711, 0.06106274],
                [0.04673809, 0.05812865, 0.07192291],
                [0.05778475, 0.04384917, 0.07802191],
                [0.06474301, 0.02708319, 0.08045689],
                [0.06844646, 0.00895837, 0.08046731],
                [0.07067417, -0.00961522, 0.08035654],
                [0.07141807, -0.02827123, 0.08114384],
                [-0.05977758, -0.0447858, 0.04562813],
                [-0.05055506, -0.05334294, 0.03834846],
                [-0.0375633, -0.05609241, 0.03158344],
                [-0.02423648, -0.05463779, 0.02510117],
                [-0.01168798, -0.04986641, 0.02050337],
                [0.01168798, -0.04986641, 0.02050337],
                [0.02423648, -0.05463779, 0.02510117],
                [0.0375633, -0.05609241, 0.03158344],
                [0.05055506, -0.05334294, 0.03834846],
                [0.05977758, -0.0447858, 0.04562813],
                [0.0, -0.03515768, 0.02038099],
                [0.0, -0.02350421, 0.01366667],
                [0.0, -0.01196914, 0.00658284],
                [0.0, 0.0, 0.0],
                [-0.01479319, 0.00949072, 0.01708772],
                [-0.00762319, 0.01179908, 0.01419133],
                [0.0, 0.01381676, 0.01205559],
                [0.00762319, 0.01179908, 0.01419133],
                [0.01479319, 0.00949072, 0.01708772],
                [-0.045, -0.032415, 0.03976718],
                [-0.0370546, -0.0371723, 0.03579593],
                [-0.0275166, -0.03714814, 0.03425518],
                [-0.01919724, -0.03101962, 0.03359268],
                [-0.02813814, -0.0294397, 0.03345652],
                [-0.03763013, -0.02948442, 0.03497732],
                [0.01919724, -0.03101962, 0.03359268],
                [0.0275166, -0.03714814, 0.03425518],
                [0.0370546, -0.0371723, 0.03579593],
                [0.045, -0.032415, 0.03976718],
                [0.03763013, -0.02948442, 0.03497732],
                [0.02813814, -0.0294397, 0.03345652],
                [-0.02847002, 0.03331642, 0.03667993],
                [-0.01796181, 0.02843251, 0.02335485],
                [-0.00742947, 0.0258057, 0.01630812],
                [0.0, 0.0275555, 0.01538404],
                [0.00742947, 0.0258057, 0.01630812],
                [0.01796181, 0.02843251, 0.02335485],
                [0.02847002, 0.03331642, 0.03667993],
                [0.0183606, 0.0423393, 0.02523355],
                [0.00808323, 0.04614537, 0.01820142],
                [0.0, 0.04688623, 0.01716318],
                [-0.00808323, 0.04614537, 0.01820142],
                [-0.0183606, 0.0423393, 0.02523355],
                [-0.02409981, 0.03367606, 0.03421466],
                [-0.00756874, 0.03192644, 0.01851247],
                [0.0, 0.03263345, 0.01732347],
                [0.00756874, 0.03192644, 0.01851247],
                [0.02409981, 0.03367606, 0.03421466],
                [0.00771924, 0.03711846, 0.01940396],
                [0.0, 0.03791103, 0.0180805],
                [-0.00771924, 0.03711846, 0.01940396],
            ],
            dtype=np.float,
        )
        self.normalized_camera_matrix = np.array(
            [[1600, 0, 112], [0, 1600, 112], [0, 0, 1]], dtype=np.float32
        )
        self.camera_matrix = np.array(
            [
                [640, 0, 320],
                [0, 640, 240],
                [0, 0, 1],
            ],
            dtype=np.float32,
        )
        self.camera_matrix_inv = np.linalg.inv(self.camera_matrix)
        self.rotation_vec = None
        self.translation_vec = None
        self.face_center = None
        self.normalized_rot = None
        self.landmarks = None
        self.image = None

    def __call__(self, topic, data):
        if topic == b"image":
            self.image = data
        if topic == b"face_landmark":
            self.landmarks = self._convert_landmarks(data)

        self.estimate()

    def _convert_landmarks(self, face_landmark):
        return face_landmark[self.landmark_index, :2]

    def estimate(self):
        if self.landmarks is None or self.image is None:
            return

        self.estimate_head_pose()
        self.estimate_gaze()

    def estimate_gaze(self):
        image = self._get_normalized_image()
        image = np.expand_dims(image, axis=0)
        data = super().invoke(image)[0]
        pitch, yaw = data[0]
        normalized_gaze_vector = -np.array(
            [np.cos(pitch) * np.sin(yaw), np.sin(pitch), np.cos(pitch) * np.cos(yaw)]  # type: ignore
        )
        gaze_vector = normalized_gaze_vector @ self.normalized_rot.as_matrix()
        # ZMQ_PUB: gaze
        self.publisher.pub(
            b"gaze",
            (
                # self.rotation_vec,
                # self.translation_vec,
                # gaze_vector,
                self.projectPoints(
                    np.vstack(
                        [
                            self.LANDMARKS[30],
                            self.LANDMARKS[30] + 0.08 * gaze_vector,
                        ],
                    )
                ),
            ),
        )
        print(f"gaze:{GazeEstimator.vec_to_angel(gaze_vector)}")

    def projectPoints(self, points):
        points, _ = cv2.projectPoints(
            points,
            self.rotation_vec.as_matrix(),
            self.translation_vec,
            self.camera_matrix,
            np.zeros((1, 5)),
        )
        return points.reshape(-1, 2)

    def _get_normalized_image(self):
        self._compute_face_center()
        self._compute_normalized_rot()
        distance = np.linalg.norm(self.face_center)
        scale = GazeEstimator._get_scale_matrix(distance)
        conversion_matrix = scale @ self.normalized_rot.as_matrix()
        projection_matrix = (
            self.normalized_camera_matrix @ conversion_matrix @ self.camera_matrix_inv
        )
        image = cv2.warpPerspective(
            self.image,
            projection_matrix,
            (224, 224),
        )
        image = image.astype(np.float32) / 255.0
        mean = [0.485, 0.456, 0.406]
        std = [0.229, 0.224, 0.225]
        image[:, :, 0] -= mean[0]
        image[:, :, 0] /= std[0]
        image[:, :, 1] -= mean[1]
        image[:, :, 1] /= std[1]
        image[:, :, 2] -= mean[2]
        image[:, :, 2] /= std[2]
        return image

    def estimate_head_pose(self):
        rvec = np.zeros(3, dtype=np.float)
        tvec = np.array([0, 0, 1], dtype=np.float)
        _, rvec, tvec = cv2.solvePnP(
            self.LANDMARKS,
            self.landmarks,
            self.camera_matrix,
            np.zeros((1, 5)),
            rvec,
            tvec,
            useExtrinsicGuess=True,
            flags=cv2.SOLVEPNP_ITERATIVE,
        )
        # ZMQ_PUB: rotation
        # self.publisher.pub(b"rotation", (rvec[0], rvec[1], rvec[2]))

        rot = Rotation.from_rotvec(rvec)
        self.rotation_vec = rot
        self.translation_vec = tvec

    def _compute_normalized_rot(self):
        z_axis = util.normalize_vector(self.face_center)
        head_rot = self.rotation_vec.as_matrix()
        head_x_axis = head_rot[:, 0]
        y_axis = util.normalize_vector(np.cross(z_axis, head_x_axis))
        x_axis = util.normalize_vector(np.cross(y_axis, z_axis))
        self.normalized_rot = Rotation.from_matrix(np.vstack([x_axis, y_axis, z_axis]))

    def _compute_face_center(self):
        rot = self.rotation_vec.as_matrix()
        model3d = self.LANDMARKS @ rot.T + self.translation_vec.ravel()
        center = model3d[
            np.concatenate([self.REYE_INDICES, self.LEYE_INDICES, self.MOUTH_INDICES])
        ].mean(axis=0)
        self.face_center = center

    @staticmethod
    def _get_scale_matrix(distance):
        return np.array(
            [
                [1, 0, 0],
                [0, 1, 0],
                [0, 0, 1.0 / distance],
            ],
            dtype=np.float,
        )

    @staticmethod
    def vec_to_angel(vec):
        x, y, z = vec
        pitch = np.arcsin(-y)
        yaw = np.arctan2(-x, -z)
        return np.rad2deg(np.array([pitch, yaw]))
