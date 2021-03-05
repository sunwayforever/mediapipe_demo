#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# 2021-02-24 17:36
from message_broker.transport import Publisher, Subscriber
from .iris_landmark_detector import IrisLandmarkDetector
import util

util.config_gpu_memory(10)

if __name__ == "__main__":
    # ZMQ_SUB: iris_roi
    Subscriber().sub([b"iris_roi"], IrisLandmarkDetector()).loop()
