#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# 2021-02-22 16:00
from cv2 import cv2

from hand_landmark.hand_points import *
from face_landmark.face_points import *
from iris_landmark.iris_points import *


class WebcamDisplay(object):
    def __init__(self, backend):
        self.face_box = None
        self.image = None
        self.surface = None
        self.eye_surfaces = None
        self.palm_box = None
        self.hand_surface = None
        self.backend = backend

    def annotate_image(self):
        self.annotate_face_bounding_box()
        self.annotate_face_landmark()
        self.annotate_eye_landmark()
        self.annotate_palm_bounding_box()
        self.annotate_hand_landmark()

    def annotate_face_landmark(self):
        if self.surface is None:
            return
        # for point in self.surface[:, :2]:
        #     cv2.circle(
        #         self.image,
        #         tuple(point),
        #         color=(255, 0, 0),
        #         radius=1,
        #         thickness=1,
        #     )

        for a, b in zip(
            face_landmark_connections[::2], face_landmark_connections[1::2]
        ):
            cv2.line(
                self.image,
                (self.surface[a][0], self.surface[a][1]),
                (self.surface[b][0], self.surface[b][1]),
                color=(0, 255, 0),
            )

    def annotate_eye_landmark(self):
        if self.eye_surfaces is None:
            return

        for eye_surface in self.eye_surfaces:
            for a, b in zip(
                eye_landmark_connections[::2], eye_landmark_connections[1::2]
            ):
                cv2.line(
                    self.image,
                    (eye_surface[a][0], eye_surface[a][1]),
                    (eye_surface[b][0], eye_surface[b][1]),
                    color=(0, 0, 255),
                )

    def annotate_palm_bounding_box(self):
        if self.palm_box is None:
            return

        x1 = self.palm_box.xmin
        x2 = self.palm_box.xmin + self.palm_box.width
        y1 = self.palm_box.ymin
        y2 = self.palm_box.ymin + self.palm_box.height

        cv2.rectangle(self.image, (x1, y1), (x2, y2), (255, 0, 0), 2)
        for i, kp in enumerate(self.palm_box.keypoints):
            cv2.putText(
                self.image,
                f"{i}",
                (kp[0], kp[1]),
                cv2.FONT_HERSHEY_SIMPLEX,
                0.6,
                (0, 255, 0),
                1,
            )

        cv2.putText(
            self.image,
            "{:.2f}".format(self.palm_box.score),
            (x1, y1 - 6),
            cv2.FONT_HERSHEY_SIMPLEX,
            0.6,
            (0, 255, 0),
            2,
        )

    def annotate_face_bounding_box(self):
        if self.face_box is None:
            return

        x1 = self.face_box.xmin
        x2 = self.face_box.xmin + self.face_box.width
        y1 = self.face_box.ymin
        y2 = self.face_box.ymin + self.face_box.height

        cv2.rectangle(self.image, (x1, y1), (x2, y2), (255, 0, 0), 2)
        cv2.putText(
            self.image,
            "{:.2f}".format(self.face_box.score),
            (x1, y1 - 6),
            cv2.FONT_HERSHEY_SIMPLEX,
            0.6,
            (0, 0, 255),
            2,
        )

    def annotate_hand_landmark(self):
        if self.hand_surface is None:
            return

        for point in self.hand_surface[:, :2]:
            cv2.circle(
                self.image,
                tuple(point),
                color=(255, 255, 0),
                radius=2,
                thickness=2,
            )
        for a, b in zip(
            hand_landmark_connections[::2], hand_landmark_connections[1::2]
        ):
            cv2.line(
                self.image,
                (self.hand_surface[a][0], self.hand_surface[a][1]),
                (self.hand_surface[b][0], self.hand_surface[b][1]),
                color=(0, 255, 0),
            )

    def reset_face(self):
        self.face_box = None
        self.surface = None
        self.eye_surfaces = None

    def reset_palm(self):
        self.palm_box = None
        self.hand_surface = None

    def __call__(self, topic, data):
        if topic == b"face_box":
            self.face_box = data

        if topic == b"face_reset":
            self.reset_face()

        if topic == b"palm_reset":
            self.reset_palm()

        if topic == b"face_landmark":
            self.surface = data

        if topic == b"image":
            self.image = data

        if topic == b"eye_landmark":
            self.eye_surfaces = data

        if topic == b"palm_box":
            self.palm_box = data

        if topic == b"hand_landmark":
            self.hand_surface = data

        if self.image is None:
            return

        self.annotate_image()
        self.backend.update_webcam_image(self.image)
