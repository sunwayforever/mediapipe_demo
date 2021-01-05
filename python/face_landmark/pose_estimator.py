#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# 2020-12-17 17:41
import numpy as np
import cv2
import math


class PoseEstimator:
    def __init__(self, img_size=(480, 640)):
        self.size = img_size

        # 3D model points.
        self.model_points = np.load("model_points.npy")

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
        self.dist_coeffs = np.zeros((4, 1))

    def solve(self, image_points):
        _, rotation_vector, translation_vector = cv2.solvePnP(
            self.model_points,
            image_points,
            self.camera_matrix,
            self.dist_coeffs,
            # rvec=self.r_vec,
            # tvec=self.t_vec,
            # useExtrinsicGuess=True,
        )
        return (rotation_vector, translation_vector)

    def get3dof(self, rvec, tvec):
        return (rvec * 180 / np.pi).reshape(-1).astype(np.int)

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
