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


def square_rect(x1, y1, x2, y2):
    center_x = (x1 + x2) // 2
    center_y = (y1 + y2) // 2
    width, height = x2 - x1, y2 - y1

    width = max(width, height)
    height = width

    new_x1 = center_x - width // 2
    new_x2 = center_x + width // 2
    new_y1 = center_y - height // 2
    new_y2 = center_y + height // 2

    return new_x1, new_y1, new_x2, new_y2


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


def remap(x, range_a, range_b):
    return (x - range_a[0]) / (range_a[1] - range_a[0] + 1e-6) * (
        range_b[1] - range_b[0]
    ) + range_b[0]


def sigmoid(x):
    return 1.0 / (1.0 + np.exp(-x))


def compute_distance(a, b):
    return distance.cosine(a, b)


def get_resource(f):
    path = os.path.join(os.path.dirname(os.path.dirname(os.path.abspath(__file__))), f)
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


def draw_border(img, pt1, pt2, color, thickness, r, d):
    x1, y1 = pt1
    x2, y2 = pt2
    # Top left
    cv2.line(img, (x1 + r, y1), (x1 + r + d, y1), color, thickness)
    cv2.line(img, (x1, y1 + r), (x1, y1 + r + d), color, thickness)
    cv2.ellipse(img, (x1 + r, y1 + r), (r, r), 180, 0, 90, color, thickness)
    # Top right
    cv2.line(img, (x2 - r, y1), (x2 - r - d, y1), color, thickness)
    cv2.line(img, (x2, y1 + r), (x2, y1 + r + d), color, thickness)
    cv2.ellipse(img, (x2 - r, y1 + r), (r, r), 270, 0, 90, color, thickness)
    # Bottom left
    cv2.line(img, (x1 + r, y2), (x1 + r + d, y2), color, thickness)
    cv2.line(img, (x1, y2 - r), (x1, y2 - r - d), color, thickness)
    cv2.ellipse(img, (x1 + r, y2 - r), (r, r), 90, 0, 90, color, thickness)
    # Bottom right
    cv2.line(img, (x2 - r, y2), (x2 - r - d, y2), color, thickness)
    cv2.line(img, (x2, y2 - r), (x2, y2 - r - d), color, thickness)
    cv2.ellipse(img, (x2 - r, y2 - r), (r, r), 0, 0, 90, color, thickness)


def normalize_vector(vector):
    return vector / np.linalg.norm(vector)
