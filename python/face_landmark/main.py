#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# 2021-02-23 10:20
from message_broker.transport import Publisher, Subscriber
from .face_landmark_detector import FaceLandmarkDetector

if __name__ == "__main__":
    Subscriber(["face"], FaceLandmarkDetector()).loop()
