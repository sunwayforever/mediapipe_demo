#!/usr/bin/env python3
# -*- coding: utf-8 -*-
import math
import cv2
import numpy as np
from collections import namedtuple

from .config import *
from message_broker.transport import Publisher
from box_detection.box_detector import BoxDetector, BoxConfig

import util

PalmROI = namedtuple("PalmROI", ["image", "mat"])


class PalmDetector(object):
    def __init__(self):
        palm_box_config = BoxConfig(
            PALM_MODEL,
            IMG_HEIGHT,
            IMG_WIDTH,
            NUM_COORDS,
            NUM_BOXES,
            MIN_SCORE_THRESH,
            NMS_THRESH,
            NUM_KEYPOINT,
        )
        self.publisher = Publisher()
        self.box_detector = BoxDetector(palm_box_config)
        self.image = None
        self.box = None

    def __call__(self, topic, data):
        if topic == b"image":
            self.image = data
            self.detect()

    def detect(self):
        boxes = self.box_detector.detect(self.image)
        if not boxes:
            # ZMQ_PUB: palm_reset
            self.publisher.pub(b"palm_reset")
            return
        # NEXT: only one palm is detected
        self.box = boxes[0]
        # ZMQ_PUB: palm_box
        self.publisher.pub(b"palm_box", self.box)
