#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# 2021-04-04 16:41
import os
import argparse
import cv2
import tensorflow as tf
import numpy as np

from model import SSDModel
from voc_dataset import ID_TO_NAME
from box import gen_anchors, decode, nms, Box
from backbone import N_ANCHORS
from config import *

parser = argparse.ArgumentParser()
parser.add_argument("image", type=str)
args = parser.parse_args()


if __name__ == "__main__":
    if not os.path.exists(f"{MODEL_WEIGHTS}.index"):
        raise (Exception(f"{MODEL_WEIGHTS} doesn't exist"))
    model = SSDModel(resume=True)

    image = cv2.imread(args.image, cv2.COLOR_BGR2RGB)
    height, width, _ = image.shape

    img = cv2.resize(image, (IMAGE_SIZE, IMAGE_SIZE))
    img = (img - 127.5) / 127.5
    img = np.expand_dims(img, 0)

    confs, locs = model(img, training=False)
    confs, locs = np.squeeze(confs), np.squeeze(locs)
    confs = tf.math.softmax(confs, axis=-1)

    scores = np.max(confs, axis=-1)
    classes = np.argmax(confs, axis=-1)

    anchors = gen_anchors()
    locs = decode(anchors, locs)
    locs = (locs * np.array([width, height, width, height])).astype("int")

    boxes = []
    for i in range(N_ANCHORS):
        if classes[i] == 0:
            continue
        if scores[i] < 0.2:
            continue
        label = ID_TO_NAME[classes[i] - 1]
        boxes.append(Box(label=label, score=scores[i], box=locs[i]))

    boxes = nms(boxes)
    for box in boxes:
        label, score = box[0], box[1]
        x1, y1, x2, y2 = box[2]
        cv2.putText(
            image,
            f"{label}:{score:.2f}",
            (x1 + 10, y1 + 15),
            cv2.FONT_HERSHEY_SIMPLEX,
            0.5,
            (0, 0, 255),
            1,
            cv2.LINE_AA,
        )
        cv2.rectangle(
            image,
            (x1, y1),
            (x2, y2),
            (0, 0, 255),
            1,
            1,
        )
    cv2.imshow("", image)
    cv2.waitKey(0)
