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
        image_height, image_width = image.shape[:2]
        image = cv2.resize(image, (IMG_WIDTH, IMG_HEIGHT))
        image = np.expand_dims(image, axis=0)
        boxes, classes, scores, count = super().invoke(image)

        count = count.astype("int")[0]
        classes = classes.astype("int").ravel()
        scores = scores.ravel()
        boxes = boxes.squeeze()
        results = []
        for i in range(count):
            if scores[i] < MIN_SCORE_THRESH:
                continue
            ymin, xmin, ymax, xmax = boxes[i]
            result = (
                (
                    int(xmin * image_width),
                    int(ymin * image_height),
                    int(xmax * image_width),
                    int(ymax * image_height),
                ),
                LABELS.get(classes[i], "unknown"),
                scores[i],
            )
            results.append(result)
        # ZMQ_PUB: objects
        self.publisher.pub(b"objects", results)
