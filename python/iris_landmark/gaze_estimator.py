#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# 2021-03-11 22:01
import numpy as np

from .iris_points import *
from common import util, PointVelocityFilter


class GazeEstimator(object):
    def __init__(self):
        self.x_degree_range = [-60, 60]
        self.x_delta_range = [-5, 5]
        self.y_range = [-30, 30]
        self.y_delta_range = [-3, 3]
        self.point_velocity_filter = PointVelocityFilter(cov_measure=0.05)

    def estimate(self, eye_surfaces, iris_surfaces):
        # shape of iris_surfaces and eye_surfaces: (2, N, 3)
        eye_centers = np.mean(eye_surfaces[:, eye_points, :2], axis=1)
        iris_centers = np.mean(iris_surfaces[:, :, :2], axis=1)
        delta = np.mean(iris_centers - eye_centers, axis=0)
        degree = np.array(
            [util.remap(delta[0], self.x_delta_range, self.x_degree_range), 0.0],
            dtype=np.float32,
        )
        x_degree, _ = self.point_velocity_filter.update(degree)
        # NEXT: gaze estimate: y_degree
        return (x_degree, 0)
