#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# 2021-03-01 20:55
import time
from config import *


class Throttler(object):
    def __init__(self):
        self.throttle = {
            # webcam
            b"image": FPS,
            b"face_box": FPS,
            b"face_reset": FPS,
            b"face_box": FPS,
            b"face_roi": FPS,
            b"face_landmark": FPS,
            b"eye_landmark": FPS,
            b"iris_landmark": FPS,
            b"eye_aspect_ratio": FPS,
            b"mouth_aspect_ratio": FPS,
            b"iris_roi": FPS,
            b"rotation": FPS,
            b"palm_reset": FPS,
            b"palm_box": FPS,
            b"palm_roi": FPS,
            b"hand_landmark": FPS,
            b"hand_gesture": FPS,
            # facenet
            b"face_roi_slow": 5,
            b"facenet": 5,
            b"enroll": 5,
        }

        self.next_time = {}

    def is_send_allowed(self, topic):
        current = round(time.time() * 1000)
        next = self.next_time.get(topic, current)
        if current < next:
            return False
        if topic not in self.throttle:
            raise (Exception("unknown topic:", topic))
        self.next_time[topic] = current + 1000 // self.throttle[topic]
        return True

    def is_recv_allowed(self, topic, time_stamp):
        current = round(time.time() * 1000)
        return current - time_stamp < 1000 // self.throttle[topic]
