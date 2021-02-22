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
        self.image.fill(Qt.red)  # Here's the magic...

    def set_image(self, image):
        self.image = image

    def requestImage(self, id, size):
        return self.image, self.image.size()


class Backend(QObject):
    imageChanged = pyqtSignal()

    def __init__(self):
        QObject.__init__(self)
        self.webcam_image_provider = WebcamImageProvider()

    def update_webcam_image(self, image):
        self.webcam_image_provider.set_image(image)
        self.imageChanged.emit()
