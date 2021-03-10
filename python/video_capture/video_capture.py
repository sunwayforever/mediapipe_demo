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
        # NEXT: it seems image receivers only have little chance to be scheduled
        # w/o the sleep, maybe the `read_bgr_image` works in a manner of `busy waiting`
        time.sleep(1 / FPS)
        image = inu_stream.read_bgr_image(self.height * self.width * 3)  # type: ignore
        if image is None:
            return None
        return cv2.cvtColor(
            np.reshape(
                image,
                (self.height, self.width, 3),
            ),
            cv2.COLOR_BGR2RGB,
        )
