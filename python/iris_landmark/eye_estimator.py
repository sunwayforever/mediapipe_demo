#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# 2021-02-24 09:47
from common import util

from .iris_points import *


class EyeEstimator(object):
    def __init__(self):
        pass

    def estimate(self, eye_image_points):
        return [
            util.get_aspect_ratio(*image_points) for image_points in eye_image_points
        ]
