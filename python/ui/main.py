#!/usr/bin/env python3
# -*- coding: utf-8 -*-
from sys import argv, exit

from PyQt5.QtGui import QGuiApplication, QImage
from PyQt5.QtQml import QQmlApplicationEngine
from PyQt5.QtCore import QRunnable, QThreadPool, Qt, pyqtSignal, QObject

from message_broker.transport import QThreadedSubscriber
from .webcam_display import WebcamDisplayCallback
from .facenet_display import FacenetDisplayCallback
from .rotation_display import RotationDisplayCallback
from .expression_display import ExpressionDisplayCallback
from .hand_gesture_display import HandGestureDisplayCallback
from .backend import *


if __name__ == "__main__":
    backend = Backend()

    subscriber = QThreadedSubscriber()

    # ZMQ_SUB: image face_box face_landmark eye_landmark face_reset palm_box palm_reset hand_landmark hand_gesture
    subscriber.sub(
        [
            b"image",
            b"face_box",
            b"face_landmark",
            b"eye_landmark",
            b"face_reset",
            b"palm_box",
            b"palm_reset",
            b"hand_landmark",
        ],
        WebcamDisplayCallback(backend),
    )

    # ZMQ_SUB: rotation
    subscriber.sub([b"rotation"], RotationDisplayCallback(backend))

    # ZMQ_SUB: mouth_aspect_ratio, eye_aspect_ratio
    subscriber.sub(
        [b"mouth_aspect_ratio", b"eye_aspect_ratio"], ExpressionDisplayCallback(backend)
    )

    # ZMQ_SUB: facenet face_reset
    subscriber.sub([b"facenet", b"face_reset"], FacenetDisplayCallback(backend))

    # ZMQ_SUB: hand_gesture palm_reset
    subscriber.sub(
        [b"hand_gesture", b"palm_reset"], HandGestureDisplayCallback(backend)
    )

    subscriber.loop()

    app = QGuiApplication(argv)
    engine = QQmlApplicationEngine()

    engine.addImageProvider("webcam", backend.webcam_image_provider)
    engine.addImageProvider("facenet", backend.facenet_image_provider)
    engine.addImageProvider("hand", backend.hand_image_provider)
    engine.rootContext().setContextProperty("backend", backend)
    engine.load("ui/main.qml")

    exit(app.exec_())
