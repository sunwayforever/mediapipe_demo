#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# 2021-03-02 12:24
# MODEL = "../model/palm_detection.onnx"
MODEL = "../model/palm_detection"

IMG_HEIGHT, IMG_WIDTH = 128, 128
NUM_COORDS = 18
NUM_BOXES = 896
MIN_SCORE_THRESH = 0.8
NMS_THRESH = 0.5
NUM_KEYPOINT = 7
