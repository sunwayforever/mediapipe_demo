#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# 2020-12-22 19:18
import cv2
import numpy as np


class Stablizer:
    def __init__(self, state_num=2, measure_num=1, cov_process=0.01, cov_measure=0.01):

        # Store the parameters.
        # 隐藏状态
        # X 为 [[x][v_x]
        self.state_num = state_num
        # 观测状态
        # Z 为 [[x]]
        self.measure_num = measure_num

        # The filter itself.
        self.filter = cv2.KalmanFilter(state_num, measure_num, 0)

        # 转移矩阵: FX+Q
        # F
        self.filter.transitionMatrix = np.array([[1, 1], [0, 1]], np.float32)
        # Q
        self.filter.processNoiseCov = np.eye(state_num, dtype=np.float32) * cov_process
        # 观测矩阵: HX+R
        # H, 由于 measure 信息里只有 x, 不包括 v_x, 所以需要用 H
        # 把 x 中的后一个数据 mask 掉
        self.filter.measurementMatrix = np.eye(measure_num, state_num, dtype=np.float32)
        # R
        self.filter.measurementNoiseCov = (
            np.eye(measure_num, dtype=np.float32) * cov_measure
        )

    def update(self, measurement):
        self.filter.predict()
        self.filter.correct(measurement)
        # statePost 是 correct 之后的 state
        # statePre 是 predict 之后, corrent 之前的 state
        return self.filter.statePost[0]


if __name__ == "__main__":
    kalman_filter = Stabilizer(2, 1)
    print(kalman_filter.update(np.array([1.0], dtype=np.float32)))
