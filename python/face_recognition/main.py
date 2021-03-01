#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# 2021-02-26 17:58
from .face_recognizer import FaceRecognizer
from message_broker.transport import Subscriber

if __name__ == "__main__":
    # ZMQ_SUB: face_roi_slow, enroll
    Subscriber().sub([b"face_roi_low_freq", b"enroll"], FaceRecognizer()).loop()
