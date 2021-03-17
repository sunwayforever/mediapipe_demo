#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# 2020-12-17 17:41
import numpy as np
from cv2 import cv2
from scipy.spatial.transform import Rotation

from common import util


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
                [640, 0, 320],
                [0, 640, 240],
                [0, 0, 1],
            ],
            dtype="double",
        )
        self.dist_coeffs = np.zeros((4, 1))
        self.rot = None

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

        self.rot = Rotation.from_rotvec(self.rotation_vector.ravel())
        self.center_3d = self._compute_center_3d()
        self.normalized_rot = self._compute_normalized_rot()

    def _compute_normalized_rot(self):
        z_axis = util.normalize_vector(self.center_3d)
        head_rot = self.rot.as_matrix()
        head_x_axis = head_rot[:, 0]
        y_axis = util.normalize_vector(np.cross(z_axis, head_x_axis))
        x_axis = util.normalize_vector(np.cross(y_axis, z_axis))
        return Rotation.from_matrix(np.vstack([x_axis, y_axis, z_axis]))

    def _compute_center_3d(self):
        rot = self.rot.as_matrix()
        model3d = self.model_points @ rot.T + self.translation_vector.ravel()
        return model3d[2].ravel()  # nose

    def get_normalized_rot(self):
        return self.normalized_rot

    def get_distance(self):
        return np.linalg.norm(self.center_3d)

    def get_rotation_degree(self):
        rvec = self.rotation_vector.ravel()
        return (rvec[0], rvec[1], rvec[2])

    def estimate(self, image_points):
        self._solve(image_points)
        return self.get_rotation_degree()
