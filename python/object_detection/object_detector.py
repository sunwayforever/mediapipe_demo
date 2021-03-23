#!/usr/bin/env python3
# -*- coding: utf-8 -*-
import cv2
import numpy as np

from .config import *
from .labels import LABELS
from message_broker.transport import Publisher

from common import util, Detector


class ObjectDetector(Detector):
    def __init__(self):
        super().__init__(util.get_resource(MODEL), {"tflite": [0, 1, 2, 3]})
        self.publisher = Publisher()

    def __call__(self, topic, data):
        if topic == b"image":
            self.detect(data)

    def detect(self, image):
        image, mat = util.resize_and_keep_aspect_ratio(image, IMG_WIDTH, IMG_HEIGHT)
        image = np.expand_dims(image, axis=0)
        boxes, classes, scores, count = super().invoke(image)

        count = count.astype("int")[0]
        classes = classes.astype("int").ravel()
        scores = scores.ravel()
        # box shape: [1, N, 4], and -1 axis is [ymin,xmin,ymax,xmax],
        # but opencv requires [xmin,ymin,xmax,ymax]
        boxes = boxes.reshape(-1, 2, 2)[:, :, ::-1]
        results = []
        rect = np.array([IMG_HEIGHT, IMG_WIDTH])
        for i in range(count):
            if scores[i] < MIN_SCORE_THRESH:
                continue
            box = util.restore_coordinates_2d(boxes[i] * rect, mat).astype("int")
            result = (
                box,
                LABELS.get(classes[i], "unknown"),
                scores[i],
            )
            results.append(result)
        # ZMQ_PUB: objects
        self.publisher.pub(b"objects", results)
