#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# 2020-12-17 17:41
import numpy as np
from cv2 import cv2


class PoseEstimator:
    def __init__(self):
        # 3D model points.

        # left eye right corner
        # right eye left corne
        # nose
        # left mouth corner
        # right mouth corner
        # chin

        self.model_points = np.array(
            [
                [392, 231, -25],
                [443, 231, -25],
                [421, 291, 0],
                [381, 350, -25],
                [449, 350, -25],
                [417, 360, -24],
            ],
            dtype=np.float32,
        )

        self.camera_matrix = np.array(
            [
                [1010, 0, 628],
                [0, 1014, 339],
                [0, 0, 1],
            ],
            dtype="double",
        )
        self.dist_coeffs = np.zeros((4, 1))

    def _solve(self, image_points):
        if hasattr(self, "rotation_vector"):
            (_, self.rotation_vector, self.translation_vector,) = cv2.solvePnP(
                self.model_points,
                image_points,
                self.camera_matrix,
                self.dist_coeffs,
                flags=cv2.SOLVEPNP_ITERATIVE,
                rvec=self.rotation_vector,
                tvec=self.translation_vector,
                useExtrinsicGuess=True,
            )
        else:
            (_, self.rotation_vector, self.translation_vector,) = cv2.solvePnP(
                self.model_points,
                image_points,
                self.camera_matrix,
                self.dist_coeffs,
                flags=cv2.SOLVEPNP_ITERATIVE,
                useExtrinsicGuess=False,
            )

    def _get3dof(self, rvec, tvec):
        rvec = rvec.ravel()
        return (rvec[0], rvec[1], rvec[2] + np.pi)

    def estimate(self, image_points):
        self._solve(image_points)
        return self._get3dof(self.rotation_vector, self.translation_vector)
