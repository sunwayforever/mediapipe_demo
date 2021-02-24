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
    router.add_route([b"image", b"box", b"mesh"], FaceDisplayCallback(backend))
    router.add_route([b"rotation"], RotationDisplayCallback(backend))
    router.add_route([b"mouth"], ExpressionDisplayCallback(backend))
    router.start()

    app = QGuiApplication(argv)
    engine = QQmlApplicationEngine()

    engine.addImageProvider("webcam", backend.webcam_image_provider)
    engine.rootContext().setContextProperty("backend", backend)
    engine.load("ui/main.qml")

    exit(app.exec_())
