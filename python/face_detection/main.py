#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# 2021-02-23 10:07
from .config import *
from .face_detector import FaceDetector
from message_broker.transport import Subscriber


def run():
    Subscriber().sub([b"image"], FaceDetector()).loop()


if __name__ == "__main__":
    run()
