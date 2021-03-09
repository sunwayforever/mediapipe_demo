#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# 2020-12-22 14:58
from cv2 import cv2
import numpy as np
import os
from scipy.spatial import distance
import tensorflow as tf
from collections import namedtuple

ROIImage = namedtuple("ROIImage", ["image", "mat"])

def get_aspect_ratio(top, bottom, left, right):
    A = distance.euclidean(top, bottom)
    B = distance.euclidean(left, right)
    return A / (B + 1e-10)


def get_scale_mat(x, y):
    return np.array([[x, 0, 0], [0, y, 0], [0, 0, 1]])


def restore_coordinates(surface: np.ndarray, mat) -> np.ndarray:
    # mat: 3x3 homogeneous
    tmp_surface: np.ndarray = surface[:, :2]
    tmp_surface = np.concatenate(
        (tmp_surface, np.ones((tmp_surface.shape[0], 1))), axis=1
    )
    tmp_surface = (mat @ tmp_surface.T).T
    tmp_surface = tmp_surface[:, :2]
    surface = np.concatenate((tmp_surface, surface[:, 2:3]), axis=1)
    return surface


def get_translation_mat(x, y):
    return np.array([[1, 0, x], [0, 1, y], [0, 0, 1]])


def remap(x, lo, hi, scale):
    return (x - lo) / (hi - lo + 1e-6) * scale


def sigmoid(x):
    return 1.0 / (1.0 + np.exp(-x))


def compute_distance(a, b):
    return distance.cosine(a, b)


def get_resource(f):
    path = os.path.join(os.path.dirname(os.path.abspath(__file__)), f)
    return path


def resize(img, roi_width, roi_height):
    # height/width
    orig_aspect_ratio = img.shape[0] / img.shape[1]
    roi_aspect_ratio = roi_height / roi_width
    h_padding, v_padding = 0, 0

    if orig_aspect_ratio < roi_aspect_ratio:
        new_width = roi_width
        new_height = int(roi_width * orig_aspect_ratio)
        v_padding = roi_height - new_height
    else:
        new_height = roi_height
        new_width = int(roi_height / orig_aspect_ratio)
        h_padding = roi_width - new_width

    img = cv2.resize(img, (int(new_width), int(new_height)))
    v_padding = int(v_padding / 2)
    h_padding = int(h_padding / 2)
    img = cv2.copyMakeBorder(
        img,
        v_padding,
        v_padding,
        h_padding,
        h_padding,
        cv2.BORDER_CONSTANT,
        value=(0, 0, 0),
    )
    return img, v_padding / roi_width, h_padding / roi_height


def config_gpu_memory(gpu_mem_cap):
    gpus = tf.config.experimental.list_physical_devices("GPU")
    if not gpus:
        return
    for gpu in gpus:
        try:
            tf.config.experimental.set_virtual_device_configuration(
                gpu,
                [
                    tf.config.experimental.VirtualDeviceConfiguration(
                        memory_limit=gpu_mem_cap
                    )
                ],
            )
        except RuntimeError as e:
            print("Can not set GPU memory config", e)


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
