#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# 2021-02-26 17:58
from .face_recognizer import FaceRecognizer
from message_broker.transport import PolledSubscriber


def run():
    # ZMQ_SUB: face_roi_slow, enroll
    face_recognizer = FaceRecognizer()
    subscriber = PolledSubscriber()
    subscriber.sub(b"face_roi_slow", face_recognizer)
    subscriber.sub(b"enroll", face_recognizer)
    subscriber.loop()


if __name__ == "__main__":
    run()
