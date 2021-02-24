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


def remap(x, lo, hi, scale):
    return (x - lo) / (hi - lo + 1e-6) * scale


def sigmoid(x):
    return 1.0 / (1.0 + np.exp(-x))


def get_resource(f):
    return os.path.join(os.path.dirname(os.path.abspath(__file__)), f)


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


prev_times = defaultdict(lambda: -1)
benchmarks = {}


def benchmark_begin(
    category,
):
    global prev_times
    prev_times[category] = time.time()


def benchmark_end(category):
    global benchmarks
    curr = time.time()
    delta = int((curr - prev_times[category]) * 1000)
    benchmarks[category] = delta


def show_benchmark(img):
    global benchmarks
    y = 0
    for category, v in benchmarks.items():
        y += 20
        cv2.putText(
            img,
            f"{category}: {v} ms",
            (20, y),
            cv2.FONT_HERSHEY_SIMPLEX,
            0.6,
            (0, 0, 255),
            2,
        )


if __name__ == "__main__":
    vid = cv2.VideoCapture(0)
    while True:
        _, orig_img = vid.read()
        img, _, _ = resize(orig_img, 128, 128)
        img2 = cv2.resize(orig_img, (128, 128))
        cv2.imshow("", np.concatenate([img, img2]))
        if cv2.waitKey(1) & 0xFF == ord("q"):
            break

    vid.release()
    cv2.destroyAllWindows()

    # parser = argparse.ArgumentParser()
    # parser.add_argument("file_name", type=str)
    # flags = parser.parse_args()
    # orig_img = cv2.imread(flags.file_name)
    # img, _, _ = resize(orig_img, 128, 128)
    # img2 = cv2.resize(orig_img, (128, 128))
    # cv2.imshow("", np.concatenate([img, img2]))
    # while cv2.waitKey(1) & 0xFF != ord("q"):
    #     pass
