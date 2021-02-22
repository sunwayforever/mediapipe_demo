#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# 2021-02-22 16:00
from PyQt5.QtGui import QGuiApplication, QImage
from PyQt5.QtQml import QQmlApplicationEngine
from PyQt5.QtCore import QRunnable, QThreadPool, Qt, pyqtSignal, QObject
import cv2


class FaceDetectionCallback(object):
    def __init__(self, backend):
        self.box = None
        self.image = None
        self.backend = backend

    def annotate_image(self):
        if self.box is None:
            return
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

        if topic == b"image":
            self.image = data
            self.annotate_image()
            image = QImage(
                self.image,
                self.image.shape[1],
                self.image.shape[0],
                self.image.shape[1] * 3,
                QImage.Format_RGB888,
            )
            self.backend.update_webcam_image(image)
