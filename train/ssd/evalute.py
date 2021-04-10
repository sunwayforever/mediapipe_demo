#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# 2021-04-04 16:41
import os
import argparse
import cv2
import tensorflow as tf
import numpy as np

from model import SSDModel
from config import *

parser = argparse.ArgumentParser()
parser.add_argument("image", type=str)
args = parser.parse_args()


if __name__ == "__main__":
    if not os.path.exists(f"{MODEL_WEIGHTS}.index"):
        raise (Exception(f"{MODEL_WEIGHTS} doesn't exist"))
    model = SSDModel(resume=True)

    img = cv2.imread(args.image, cv2.COLOR_BGR2RGB)
    img = cv2.resize(img, (IMAGE_SIZE, IMAGE_SIZE))
    img = (img - 127.5) / 127.5
    img = np.expand_dims(img, 0)

    confs, locs = model(img, training=False)
    confs, locs = np.squeeze(confs), np.squeeze(locs)
    confs = tf.math.softmax(confs, axis=-1)

    import ipdb; ipdb.set_trace()
    scores = np.max(confs, axis=-1)
    classes = np.argmax(confs, axis=-1)

    for i in range(8732):
        if classes[i] != 0:
            print(f"{i}:{classes[i]}:{scores[i]}")
