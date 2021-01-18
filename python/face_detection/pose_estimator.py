#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# 2020-12-17 17:41
import numpy as np
import cv2

from .stablizer import Stablizer

class PoseEstimator:
    def __init__(self, img_size=(480, 640)):
        self.size = img_size

        # 3D model points.
        self.model_points = (
            np.array(
                [
                    (-225.0, 170.0, -135.0),  # Left eye
                    (225.0, 170.0, -135.0),  # Right eye
                    (0.0, 0.0, 0.0),  # Nose tip
                    (0.0, -150.0, -125.0),  # Mouth
                    (-500.0, 140.0, -170.0),
                    (500.0, 140.0, -170.0),
                ]
            )
            / 2
        )

        self.focal_length = self.size[1]
        self.camera_center = (self.size[1] / 2, self.size[0] / 2)
        self.camera_matrix = np.array(
            [
                [self.focal_length, 0, self.camera_center[0]],
                [0, self.focal_length, self.camera_center[1]],
                [0, 0, 1],
            ],
            dtype="double",
        )

        # Assuming no lens distortion
        self.dist_coeffs = np.zeros((4, 1))
        self.rotation_stablizers = [Stablizer(2, 1) for _ in range(3)]
        self.translation_stablizers = [Stablizer(2, 1) for _ in range(3)]

        self.r_vec = np.array([[-2.3702784], [-0.74427134], [-0.5183508]])
        self.t_vec = np.array([[173.36334], [-17.135548], [904.95544]])

    def solve(self, image_points):
        # if self.r_vec is None:
        #     _, rotation_vector, translation_vector = cv2.solvePnP(
        #         self.model_points,
        #         image_points,
        #         self.camera_matrix,
        #         self.dist_coeffs,
        #     )
        #     self.r_vec = rotation_vector
        #     self.t_vec = translation_vector

        _, rotation_vector, translation_vector = cv2.solvePnP(
            self.model_points,
            image_points,
            self.camera_matrix,
            self.dist_coeffs,
            rvec=self.r_vec,
            tvec=self.t_vec,
            useExtrinsicGuess=True,
        )

        rotation_vector = np.array(
            [
                stablizer.update(np.array([x], dtype=np.float32))
                for x, stablizer in zip(
                    rotation_vector.flatten(), self.rotation_stablizers
                )
            ]
        )
        translation_vector = np.array(
            [
                stablizer.update(np.array([x], dtype=np.float32))
                for x, stablizer in zip(
                    translation_vector.flatten(), self.translation_stablizers
                )
            ]
        )
        # print(rotation_vector)
        # print(translation_vector)
        return (rotation_vector, translation_vector)

    def draw_stick(
        self,
        image,
        points,
        rotation_vector,
        translation_vector,
    ):
        (nose_end, _) = cv2.projectPoints(
            np.array([(0.0, 0.0, 500.0)]),
            rotation_vector,
            translation_vector,
            self.camera_matrix,
            self.dist_coeffs,
        )

        p1 = (int(points[2][0]), int(points[2][1]))
        p2 = (int(nose_end[0][0][0]), int(nose_end[0][0][1]))
        cv2.line(image, p1, p2, (0, 255, 0), 2)
