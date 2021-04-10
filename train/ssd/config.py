#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# 2021-03-30 20:53
import os

N_ANCHOR_LEVELS = 6
ANCHOR_RATIOS = [[2], [2, 3], [2, 3], [2, 3], [2], [2]]
ANCHOR_SCALES = [0.1, 0.2, 0.375, 0.55, 0.725, 0.9, 1.075]
FEATURE_MAP_SIZES = [38, 19, 10, 5, 3, 1]
IMAGE_SIZE = 300
N_CLASSES = 21

HARD_NEGATIVE_RATIO = 3

SSD = os.environ.get("SSD", "default")
DATASET = "/home/user/download/VOCdevkit/VOC2007/"
MODEL_WEIGHTS = f"../../cache/ssd_weights/{SSD}"
SUMMARY_DIR = f"../../cache/ssd_summary/{SSD}"

LR = 1e-3
MOMENTUM = 0.9
WEIGHT_DECAY = 5e-4

BATCH = 32
N_BATCH = 50
EPOCH = 400

IOU_THRESHOLD = 0.5
