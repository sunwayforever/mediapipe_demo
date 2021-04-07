#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# 2021-03-30 20:55
import numpy as np
import math

from config import *

_anchors = None


def gen_anchors():
    global _anchors
    if _anchors is not None:
        return _anchors

    boxes = []
    for lvl in range(N_ANCHOR_LEVELS):
        for i in range(FEATURE_MAP_SIZES[lvl]):
            for j in range(FEATURE_MAP_SIZES[lvl]):
                cx = (j + 0.5) / FEATURE_MAP_SIZES[lvl]
                cy = (i + 0.5) / FEATURE_MAP_SIZES[lvl]
                boxes.append([cx, cy, ANCHOR_SCALES[lvl], ANCHOR_SCALES[lvl]])
                boxes.append(
                    [
                        cx,
                        cy,
                        math.sqrt(ANCHOR_SCALES[lvl] * ANCHOR_SCALES[lvl + 1]),
                        math.sqrt(ANCHOR_SCALES[lvl] * ANCHOR_SCALES[lvl + 1]),
                    ]
                )

                for ratio in ANCHOR_RATIOS[lvl]:
                    r = math.sqrt(ratio)
                    boxes.append(
                        [cx, cy, ANCHOR_SCALES[lvl] * r, ANCHOR_SCALES[lvl] / r]
                    )
                    boxes.append(
                        [cx, cy, ANCHOR_SCALES[lvl] / r, ANCHOR_SCALES[lvl] * r]
                    )

    boxes = np.array(boxes)
    _anchors = np.clip(boxes, 0.0, 1.0)
    return _anchors


def compute_area(top_left, bottom_right):
    rect = np.clip(bottom_right - top_left, 0.0, 1.0)
    return np.product(rect, -1)


def compute_iou(boxes_a, boxes_b):
    # m, 4 -> m, 1, 4
    # the expand dim is used for broadcast
    boxes_a = np.expand_dims(boxes_a, 1)
    # n, 4 -> 1, n, 4
    boxes_b = np.expand_dims(boxes_b, 0)
    # m,n,2
    top_left = np.maximum(boxes_a[:, :, :2], boxes_b[:, :, :2])
    bottom_right = np.minimum(boxes_a[:, :, 2:], boxes_b[:, :, 2:])

    overlap = compute_area(top_left, bottom_right)
    area_a = compute_area(boxes_a[:, :, :2], boxes_a[:, :, 2:])
    area_b = compute_area(boxes_b[:, :, :2], boxes_b[:, :, 2:])

    return overlap / (area_a + area_b - overlap)


def center_to_corner(boxes):
    return np.hstack([boxes[:, :2] - boxes[:, 2:] / 2, boxes[:, :2] + boxes[:, 2:] / 2])


def corner_to_center(boxes):
    return np.hstack(
        [(boxes[..., :2] + boxes[..., 2:]) / 2, boxes[..., 2:] - boxes[..., :2]]
    )


def encode(anchors, locs):
    locs = corner_to_center(locs)
    locs = np.hstack(
        [
            (locs[:, :2] - anchors[:, :2]) / anchors[:, 2:] / 0.1,
            np.log(locs[:, 2:] / anchors[:, 2:]) / 0.2,
        ]
    )
    return locs


def decode(anchors, locs):
    locs = np.hstack(
        [
            locs[:, :2] * 0.1 * anchors[:, 2:] + anchors[:, :2],
            np.exp(locs[:, 2:] * 0.2) * anchors[:, 2:],
        ],
    )

    locs = center_to_corner(locs)
    return locs


def compute_ground_truth(boxes, labels):
    anchors = gen_anchors()
    # [n_anchors, n_boxes]
    iou = compute_iou(center_to_corner(anchors), boxes)

    best_box_iou = np.max(iou, axis=1)
    best_box_index = np.argmax(iou, axis=1)
    # [n_anchors,4]
    confs = labels[best_box_index]
    confs[best_box_iou < 0.5] = 0

    locs = boxes[best_box_index]
    locs = encode(anchors, locs)
    return confs, locs


if __name__ == "__main__":
    _anchors = gen_anchors()
    # 004954
    boxes = np.array(
        [
            [0.05333333333333334, 0.306, 0.8466666666666667, 0.964],
            [0.2311111111111111, 0.06, 0.9244444444444444, 0.872],
        ]
    )
    labels = np.array([17, 15])

    compute_ground_truth(boxes, labels)
