#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# 2021-02-22 16:07
from PyQt5.QtGui import QGuiApplication, QImage
from PyQt5.QtQml import QQmlApplicationEngine
from PyQt5.QtCore import QRunnable, QThreadPool, Qt, pyqtSignal, QObject

from PyQt5.QtQuick import QQuickImageProvider


class WebcamImageProvider(QQuickImageProvider):
    def __init__(self):
        super(WebcamImageProvider, self).__init__(QQuickImageProvider.Image)
        self.image = QImage(256, 256, QImage.Format_RGBA8888)
        self.image.fill(Qt.red)

    def set_image(self, image):
        self.image = image

    def requestImage(self, id, size):
        return self.image, self.image.size()


class Backend(QObject):
    imageChanged = pyqtSignal()
    rotationChanged = pyqtSignal(list)
    mouthChanged = pyqtSignal(bool)
    eyeChanged = pyqtSignal(bool, bool)

    def __init__(self):
        QObject.__init__(self)
        self.webcam_image_provider = WebcamImageProvider()

    def update_webcam_image(self, image):
        self.webcam_image_provider.set_image(image)
        self.imageChanged.emit()

    def update_rotation(self, vector):
        self.rotationChanged.emit(vector)

    def update_mouth(self, is_open):
        self.mouthChanged.emit(is_open)

    def update_eye(self, left_close, right_close):
        self.eyeChanged.emit(left_close, right_close)
