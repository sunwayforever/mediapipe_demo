#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# 2021-02-22 16:00
from cv2 import cv2
import numpy as np
from collections import defaultdict
import time

from hand_landmark.hand_points import *
from face_landmark.face_points import *
from iris_landmark.iris_points import *
from common import util


class FPS(object):
    def __init__(self):
        self.counters = defaultdict(list)
        self.fps = defaultdict(int)
        self.N = 10

    def update(self, category):
        counter = self.counters[category]
        counter.append(int(time.time() * 1000))
        if len(counter) < self.N:
            return
        self.fps[category] = self.N * 1000 // (counter[-1] - counter[0])
        counter.pop(0)

    def reset(self, topics):
        for topic in topics:
            if topic in self.counters:
                del self.counters[topic]
            if topic in self.fps:
                del self.fps[topic]

    def get(self):
        return self.fps


class WebcamDisplay(object):
    def __init__(self, backend):
        self.fps = FPS()
        self.face_box = None
        self.image = None
        self.surface = None
        self.eye_surfaces = None
        self.iris_surfaces = None
        self.palm_box = None
        self.hand_surface = None
        self.gaze = None
        self.backend = backend

    def annotate_image(self):
        self.annotate_fps()
        self.annotate_face_landmark()
        self.annotate_face_bounding_box()
        self.annotate_iris_landmark()
        self.annotate_eye_landmark()
        self.annotate_hand_landmark()
        self.annotate_palm_bounding_box()
        self.annotate_gaze()

    def annotate_gaze(self):
        if self.gaze is None:
            return
        print("got gaze point")
        (points,) = self.gaze
        points = np.round(points).astype(np.int)
        cv2.line(
            self.image, tuple(points[0]), tuple(points[1]), (255, 0, 0), 1, cv2.LINE_AA
        )

    def annotate_fps(self):
        cv2.putText(
            self.image,
            "FPS:",
            (10, 20),
            cv2.FONT_HERSHEY_SIMPLEX,
            0.5,
            (255, 0, 0),
            1,
            cv2.LINE_AA,
        )

        for i, (k, v) in enumerate(self.fps.get().items()):
            cv2.putText(
                self.image,
                f"{k}:{v}",
                (10, 40 + 20 * i),
                cv2.FONT_HERSHEY_SIMPLEX,
                0.4,
                (255, 0, 0),
                1,
                cv2.LINE_AA,
            )

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

    def annotate_iris_landmark(self):
        if self.iris_surfaces is None:
            return
        for iris_surface in self.iris_surfaces:
            for point in iris_surface[:, :2]:
                cv2.circle(
                    self.image,
                    tuple(point),
                    color=(0xFF, 0x63, 0x47),
                    radius=1,
                    thickness=1,
                )

    def annotate_palm_bounding_box(self):
        if self.palm_box is None:
            return

        x1 = self.palm_box.xmin
        x2 = self.palm_box.xmin + self.palm_box.width
        y1 = self.palm_box.ymin
        y2 = self.palm_box.ymin + self.palm_box.height

        util.draw_border(self.image, (x1, y1), (x2, y2), (255, 0, 0), 2, 3, 5)

    def annotate_face_bounding_box(self):
        if self.face_box is None:
            return
        x1 = self.face_box.xmin
        x2 = self.face_box.xmin + self.face_box.width
        y1 = self.face_box.ymin
        y2 = self.face_box.ymin + self.face_box.height
        util.draw_border(self.image, (x1, y1), (x2, y2), (255, 0, 0), 2, 5, 10)

    def annotate_hand_landmark(self):
        if self.hand_surface is None:
            return

        for point in self.hand_surface[:, :2]:
            cv2.circle(
                self.image,
                tuple(point),
                color=(0xFF, 0xA5, 0x0),
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
        self.iris_surfaces = None
        self.gaze = None

    def reset_palm(self):
        self.palm_box = None
        self.hand_surface = None

    def __call__(self, topic, data):
        if topic == b"face_box":
            self.face_box = data
            self.fps.update("face")

        if topic == b"face_reset":
            self.reset_face()
            self.fps.reset(["face", "face_landmark", "iris_landmark"])

        if topic == b"palm_reset":
            self.reset_palm()
            self.fps.reset(["hand", "hand_landmark"])

        if topic == b"face_landmark":
            self.surface = data
            self.fps.update("face_landmark")

        if topic == b"image":
            self.image = data
            self.fps.update("webcam")

        if topic == b"eye_landmark":
            self.eye_surfaces = data

        if topic == b"iris_landmark":
            self.iris_surfaces = data
            self.fps.update("iris_landmark")

        if topic == b"gaze":
            self.gaze = data

        if topic == b"palm_box":
            self.palm_box = data
            self.fps.update("hand")

        if topic == b"hand_landmark":
            self.hand_surface = data
            self.fps.update("hand_landmark")

        if self.image is None:
            return

        self.annotate_image()
        self.backend.update_webcam_image(self.image)
