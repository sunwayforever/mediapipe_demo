#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# 2020-12-22 14:58
import cv2
import argparse
import numpy as np
import time
import os
from collections import defaultdict
from scipy.spatial import distance


def get_aspect_ratio(top, bottom, left, right):
    A = distance.euclidean(top, bottom)
    B = distance.euclidean(left, right)
    return A / (B + 1e-10)


def get_scale_mat(x, y):
    return np.array([[x, 0, 0], [0, y, 0], [0, 0, 1]])


def restore_coordinates(surface, mat):
    # mat: 3x3 homogeneous
    tmp_surface = surface[:, :2]
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
