#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# 2021-02-24 09:47
from common import util


class MouthEstimator(object):
    def __init__(self):
        pass

    def estimate(self, image_points):
        aspect_ratio = util.get_aspect_ratio(*image_points)
        return aspect_ratio
