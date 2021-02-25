#!/usr/bin/env python3
# -*- coding: utf-8 -*-
from sys import argv, exit

from PyQt5.QtGui import QGuiApplication, QImage
from PyQt5.QtQml import QQmlApplicationEngine
from PyQt5.QtCore import QRunnable, QThreadPool, Qt, pyqtSignal, QObject

from .face_display import FaceDisplayCallback
from .rotation_display import RotationDisplayCallback
from .expression_display import ExpressionDisplayCallback
from .backend import *
from .router import *

if __name__ == "__main__":
    backend = Backend()

    router = Router()

    # ZMQ_SUB: image, face_box, face_landmark eye_landmark
    router.add_route(
        [b"image", b"face_box", b"face_landmark", b"eye_landmark"],
        FaceDisplayCallback(backend),
    )

    # ZMQ_SUB: rotation
    router.add_route([b"rotation"], RotationDisplayCallback(backend))

    # ZMQ_SUB: mouth_aspect_ratio, eye_aspect_ratio
    router.add_route(
        [b"mouth_aspect_ratio", b"eye_aspect_ratio"], ExpressionDisplayCallback(backend)
    )
    router.start()

    app = QGuiApplication(argv)
    engine = QQmlApplicationEngine()

    engine.addImageProvider("webcam", backend.webcam_image_provider)
    engine.rootContext().setContextProperty("backend", backend)
    engine.load("ui/main.qml")

    exit(app.exec_())
