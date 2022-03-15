#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# 2021-03-01 20:55
import time
from config import *


class Throttler(object):
    def __init__(self):
        self.throttle = {
            # facenet
            b"face_roi_small": 1,
        }

        self.next_time = {}

    def is_send_allowed(self, topic):
        current = round(time.time() * 1000)
        next = self.next_time.get(topic, current)
        if current < next:
            return False
        self.next_time[topic] = current + 1000 // self.throttle.get(topic, FPS)
        return True
