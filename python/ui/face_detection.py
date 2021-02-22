#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# 2021-02-22 16:00
from PyQt5.QtGui import QGuiApplication, QImage
from PyQt5.QtQml import QQmlApplicationEngine
from PyQt5.QtCore import QRunnable, QThreadPool, Qt, pyqtSignal, QObject


class FaceDetectionCallback(object):
    def __init__(self, backend):
        self.box = None
        self.image = None
        self.backend = backend

    def __call__(self, topic, data):
        if topic == b"box":
            self.box = data

        if topic == b"image":
            self.image = data
            image = QImage(
                self.image,
                self.image.shape[1],
                self.image.shape[0],
                self.image.shape[1] * 3,
                QImage.Format_RGB888,
            )
            self.backend.update_webcam_image(image)
