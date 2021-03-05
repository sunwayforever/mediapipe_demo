#!/usr/bin/env python3
# -*- coding: utf-8 -*-
from sys import argv, exit

from PyQt5.QtGui import QGuiApplication
from PyQt5.QtQml import QQmlApplicationEngine

from message_broker.transport import QThreadedSubscriber
from .webcam_display import WebcamDisplay
from .facenet_display import FacenetDisplay
from .rotation_display import RotationDisplay
from .expression_display import ExpressionDisplay
from .hand_gesture_display import HandGestureDisplay
from .backend import *


if __name__ == "__main__":
    backend = Backend()

    subscriber = QThreadedSubscriber()

    # ZMQ_SUB: image face_box face_landmark eye_landmark face_reset palm_box palm_reset hand_landmark
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
        WebcamDisplay(backend),
    )

    # ZMQ_SUB: rotation
    subscriber.sub([b"rotation"], RotationDisplay(backend))

    # ZMQ_SUB: mouth_aspect_ratio, eye_aspect_ratio
    subscriber.sub(
        [b"mouth_aspect_ratio", b"eye_aspect_ratio"], ExpressionDisplay(backend)
    )

    # ZMQ_SUB: facenet face_reset
    subscriber.sub([b"facenet", b"face_reset"], FacenetDisplay(backend))

    # ZMQ_SUB: hand_gesture palm_reset
    subscriber.sub([b"hand_gesture", b"palm_reset"], HandGestureDisplay(backend))

    subscriber.loop()

    app = QGuiApplication(argv)
    engine = QQmlApplicationEngine()

    engine.addImageProvider("webcam", backend.webcam_image_provider)
    engine.addImageProvider("facenet", backend.facenet_image_provider)
    engine.addImageProvider("hand", backend.hand_image_provider)
    engine.rootContext().setContextProperty("backend", backend)
    engine.load("ui/main.qml")

    exit(app.exec_())
