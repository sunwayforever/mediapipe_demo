#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# 2020-12-17 17:41
import numpy as np
import cv2
import math
import util


class PoseEstimator:
    def __init__(self, img_size=(480, 640)):
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

        height, width = img_size[0], img_size[1]

        self.camera_matrix = np.array(
            [
                [1010, 0, 628],
                [0, 1014, 339],
                [0, 0, 1],
            ],
            dtype="double",
        )
        self.dist_coeffs = np.zeros((4, 1))

    def solve(self, image_points):
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

        return (self.rotation_vector, self.translation_vector)

    def get3dof(self, rvec, tvec):
        # R, _ = cv2.Rodrigues(rvec)
        # pitch = math.atan2(R[1, 0], R[0, 0])
        # yaw = math.atan2(
        #     -R[2, 0], math.sqrt(math.pow(R[2, 1], 2) + math.pow(R[2, 2], 2))
        # )
        # roll = math.atan2(R[2, 1], R[2, 2])
        # return int(yaw * 57), int(pitch * 57), int(roll * 57)
        return rvec[0], rvec[1], rvec[2] - np.pi

    def draw_stick(
        self,
        image,
        nose,
        rotation_vector,
        translation_vector,
    ):
        (nose_end, _) = cv2.projectPoints(
            np.array([(440.4, 225.2, 200.0)]),
            rotation_vector,
            translation_vector,
            self.camera_matrix,
            self.dist_coeffs,
        )

        p1 = (int(nose[0]), int(nose[1]))
        p2 = (int(nose_end[0][0][0]), int(nose_end[0][0][1]))
        cv2.line(image, p1, p2, (0, 0, 255), 2)

        yaw, pitch, roll = self.get3dof(rotation_vector, translation_vector)

        for index, (k, v) in enumerate(
            zip(["yaw", "pitch", "roll"], [yaw, pitch, roll])
        ):
            cv2.putText(
                image,
                f"{k}:{v}",
                (20, 100 + index * 20),
                cv2.FONT_HERSHEY_SIMPLEX,
                0.6,
                (255, 0, 0),
                2,
            )

    def estimate(self, img, surface, nose):
        pose = self.solve(surface)
        self.draw_stick(img, nose, pose[0], pose[1])
        return pose
