#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# 2021-02-20 12:41
import cv2
import numpy as np
import time

from .config import *
from config import *

class WebCamVideoCapture(object):
    def __init__(self):
        self.vid = cv2.VideoCapture(0)

    def capture(self):
        return cv2.flip(self.vid.read()[1], 2)
