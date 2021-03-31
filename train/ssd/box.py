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
    rect = bottom_right - top_left
    return np.clip(np.product(rect, -1), 0.0, 1.0)


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


def compute_groud_truth(boxes, labels):
    anchors = gen_anchors()
    # convert [cx,cy,w,h] to [xmin,ymin,xmax,ymax] for new_anchors
    new_anchors = np.hstack(
        [anchors[:, :2] - anchors[:, 2:] / 2, anchors[:, :2] + anchors[:, 2:] / 2]
    )
    # [n_anchors, n_boxes]
    iou = compute_iou(new_anchors, boxes)

    best_box_iou = np.max(iou, axis=1)
    best_box_index = np.argmax(iou, axis=1)
    # [n_anchors,4]
    confs = labels[best_box_index]
    confs[best_box_iou < 0.5] = 0

    locs = boxes[best_box_index]
    # convert [xmin,ymin,xmax,ymax] of box to [cx,cy,w,h]
    locs = np.hstack([(locs[:, :2] + locs[:, 2:]) / 2, locs[:, 2:] - locs[:, :2]])
    # center locs to new_anchors
    new_locs = np.hstack(
        [
            (locs[:, :2] - anchors[:, :2]) / anchors[:, 2:] / 0.1,
            np.log(locs[:, 2:] / anchors[:, 2:]) / 0.2,
        ]
    )
    return confs, new_locs


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

    compute_groud_truth(boxes, labels)
