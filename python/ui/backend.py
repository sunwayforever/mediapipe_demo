#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# 2021-02-22 16:07
import cv2
from PyQt5.QtGui import QGuiApplication, QImage
from PyQt5.QtQml import QQmlApplicationEngine
from PyQt5.QtCore import QRunnable, QThreadPool, Qt, QObject
from PyQt5.QtQuick import QQuickImageProvider
from PyQt5.QtCore import pyqtSignal, pyqtSlot

from message_broker.transport import Publisher
import util


class ImageProvider(QQuickImageProvider):
    def __init__(self, default_image=None):
        super(ImageProvider, self).__init__(QQuickImageProvider.Image)
        if default_image:
            default_image = cv2.cvtColor(
                cv2.imread(util.get_resource(default_image)), cv2.COLOR_BGR2RGB
            )
            self.set_image(default_image)

    def set_image(self, image):
        self.image = QImage(
            image,
            image.shape[1],
            image.shape[0],
            image.shape[1] * 3,
            QImage.Format_RGB888,
        )

    def requestImage(self, id, size):
        return self.image, self.image.size()


class Backend(QObject):
    webcamImageChanged = pyqtSignal()
    rotationChanged = pyqtSignal(list)
    mouthChanged = pyqtSignal(bool)
    eyeChanged = pyqtSignal(bool, bool)
    facenetImageChanged = pyqtSignal(bool)

    def __init__(self):
        QObject.__init__(self)
        self.publisher = Publisher()
        self.webcam_image_provider = ImageProvider("../data/blank_webcam.png")
        self.facenet_image_provider = ImageProvider("../data/unknown_face.png")

    def update_webcam_image(self, image):
        self.webcam_image_provider.set_image(image)
        self.webcamImageChanged.emit()

    def update_rotation(self, vector):
        self.rotationChanged.emit(vector)

    def update_mouth(self, is_open):
        self.mouthChanged.emit(is_open)

    def update_eye(self, left_close, right_close):
        self.eyeChanged.emit(left_close, right_close)

    def update_facenet_image(self, image, enrolled):
        self.facenet_image_provider.set_image(image)
        self.facenetImageChanged.emit(enrolled)

    @pyqtSlot()
    def enroll(self):
        # ZMQ_PUB: enroll
        self.publisher.pub(b"enroll")
