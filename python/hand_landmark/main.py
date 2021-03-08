#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# 2021-02-23 10:20
from .hand_landmark_detector import HandLandmarkDetector
from message_broker.transport import Subscriber


def run():
    # ZMQ_SUB: palm_roi
    Subscriber().sub([b"palm_roi"], HandLandmarkDetector()).loop()


if __name__ == "__main__":
    run()
