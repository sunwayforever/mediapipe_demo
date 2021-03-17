#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# 2021-02-23 10:20
from message_broker.transport import Subscriber
from .gaze_estimator import GazeEstimator


def run():
    # ZMQ_SUB: image
    Subscriber().sub([b"image", b"face_landmark"], GazeEstimator()).loop()


if __name__ == "__main__":
    run()
