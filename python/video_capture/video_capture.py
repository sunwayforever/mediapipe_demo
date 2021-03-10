#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# 2021-02-20 12:41
from cv2 import cv2
import numpy as np
import time

from .config import *
from config import *

if DEVICE == "inu":
    import inu_stream  # type: ignore


class WebCamVideoCapture(object):
    def __init__(self):
        self.vid = cv2.VideoCapture(0)

    def capture(self):
        return cv2.cvtColor(cv2.flip(self.vid.read()[1], 2), cv2.COLOR_BGR2RGB)


class InuVideoCapture(object):
    def __init__(self):
        self.height, self.width = inu_stream.shape()

    def capture(self):
        # NEXT:
        time.sleep(1 / FPS)
        return cv2.cvtColor(
            np.reshape(
                inu_stream.read_bgr_image(self.height * self.width * 3),  # type: ignore
                (self.height, self.width, 3),
            ),
            cv2.COLOR_BGR2RGB,
        )
