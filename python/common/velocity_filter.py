#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# 2021-03-09 21:16
import cv2
import numpy as np


class PointVelocityFilter:
    """Using Kalman filter as a point stabilizer."""

    def __init__(self, cov_process=0.0001, cov_measure=0.0001):

        # Store the parameters.
        # 隐藏状态
        # X 为 [[x],[y],[v_x],[v_y]]
        self.state_num = 4
        # 观测状态
        # Z 为 [[x],[y]]
        self.measure_num = 2

        # The filter itself.
        self.filter = cv2.KalmanFilter(self.state_num, self.measure_num, 0)

        # 转移矩阵: FX+Q
        # F
        self.filter.transitionMatrix = np.array(
            [[1, 0, 1, 0], [0, 1, 0, 1], [0, 0, 1, 0], [0, 0, 0, 1]], np.float32
        )
        # Q
        self.filter.processNoiseCov = np.eye(4, dtype=np.float32) * cov_process
        # 观测矩阵: HX+R
        # H, 由于 measure 信息里只有 x,y, 不包括 v_x, v_y, 所以需要用 H 把 x 中
        # 的后两个数据 mask 掉
        self.filter.measurementMatrix = np.eye(2, 4, dtype=np.float32)
        # R
        self.filter.measurementNoiseCov = np.eye(2, dtype=np.float32) * cov_measure

    def update(self, measurement):
        self.filter.predict()
        self.filter.correct(np.expand_dims(measurement, 1))
        return np.squeeze(self.filter.statePost[:2])
