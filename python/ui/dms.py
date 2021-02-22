#!/usr/bin/env python3
# -*- coding: utf-8 -*-
from sys import argv, exit

from PyQt5.QtGui import QGuiApplication, QImage
from PyQt5.QtQml import QQmlApplicationEngine
from PyQt5.QtCore import QRunnable, QThreadPool, Qt, pyqtSignal, QObject

from PyQt5.QtQuick import QQuickImageProvider

from message_bus import *
from .face_detection import *
import cv2


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


class MessageHandler(QRunnable):
    def __init__(self, port, callback):
        QRunnable.__init__(self)
        self.callback = callback
        self.subscriber = Subscriber(port, callback)

    def run(self):
        while True:
            self.subscriber.recv()

if __name__ == "__main__":
    backend = Backend()
    handlers = [MessageHandler(ZMQ_FACE_DETECTOR, FaceDetectionCallback(backend))]

    for handler in handlers:
        QThreadPool.globalInstance().start(handler)

    app = QGuiApplication(argv)
    engine = QQmlApplicationEngine()

    engine.addImageProvider("webcam", backend.webcam_image_provider)
    engine.rootContext().setContextProperty("backend", backend)
    engine.load("ui/main.qml")

    exit(app.exec_())
