#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# 2021-02-22 16:00
from PyQt5.QtGui import QGuiApplication, QImage
from PyQt5.QtQml import QQmlApplicationEngine
from PyQt5.QtCore import QRunnable, QThreadPool, Qt, pyqtSignal, QObject
import cv2
import random

from face_landmark.face_points import *


class FaceDisplayCallback(object):
    def __init__(self, backend):
        self.box = None
        self.image = None
        self.surface = None
        self.rotation = None
        self.backend = backend

    def annotate_image(self):
        self.annotate_face_bounding_box()
        self.annotate_face_landmark()

    def annotate_face_landmark(self):
        if self.surface is None:
            return

        for i in pose_points:
            cv2.circle(
                self.image,
                tuple(self.surface[i][:2]),
                color=(0, 255, 0),
                radius=2,
                thickness=2,
            )

        for i in mouth_points:
            cv2.circle(
                self.image,
                tuple(self.surface[i][:2]),
                color=(0, 0, 255),
                radius=2,
                thickness=2,
            )

        for i in left_face_points:
            cv2.circle(
                self.image,
                tuple(self.surface[i][:2]),
                color=(0, 255, 255),
                radius=2,
                thickness=2,
            )

        for i in right_face_points:
            cv2.circle(
                self.image,
                tuple(self.surface[i][:2]),
                color=(255, 0, 0),
                radius=2,
                thickness=2,
            )

        for a, b in zip(
            face_landmark_connections[::2], face_landmark_connections[1::2]
        ):
            cv2.line(
                self.image,
                (self.surface[a][0], self.surface[a][1]),
                (self.surface[b][0], self.surface[b][1]),
                color=(0, 255, 0),
            )

    def annotate_face_bounding_box(self):
        if self.box is None:
            return
        # face bounding box
        img_height = self.image.shape[0]
        img_width = self.image.shape[1]

        x1 = int(img_width * self.box.xmin)
        x2 = int(img_width * (self.box.xmin + self.box.width))
        y1 = int(img_height * self.box.ymin)
        y2 = int(img_height * (self.box.ymin + self.box.height))

        cv2.rectangle(self.image, (x1, y1), (x2, y2), (255, 0, 0), 2)
        cv2.putText(
            self.image,
            "{:.2f}".format(self.box.score),
            (x1, y1 - 6),
            cv2.FONT_HERSHEY_SIMPLEX,
            0.6,
            (0, 0, 255),
            2,
        )

    def __call__(self, topic, data):
        if topic == b"box":
            self.box = data

        if topic == b"mesh":
            self.surface = data

        if topic == b"image":
            self.image = data

        if topic == b"rotation":
            self.rotation = data

        if self.image is None:
            return

        self.annotate_image()

        image = QImage(
            self.image,
            self.image.shape[1],
            self.image.shape[0],
            self.image.shape[1] * 3,
            QImage.Format_RGB888,
        )
        self.backend.update_webcam_image(image)

        if self.rotation:
            self.backend.update_rotation(
                [
                    int(self.rotation[0] * 45),
                    int(self.rotation[1] * 45),
                    int(self.rotation[2] * 45),
                ]
            )
