#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# 2021-02-23 10:20
from message_broker.transport import Subscriber
from .face_landmark_detector import FaceLandmarkDetector
import util

util.config_gpu_memory(10)

if __name__ == "__main__":
    # ZMQ_SUB: face_roi
    Subscriber().sub([b"face_roi"], FaceLandmarkDetector()).loop()
