#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# 2021-02-26 17:58
from .face_recognizer import FaceRecognizer
from message_broker import Subscriber


def run():
    # ZMQ_SUB: face_roi_small, enroll
    Subscriber().sub([b"face_roi_small", b"enroll"], FaceRecognizer()).loop()


if __name__ == "__main__":
    run()
