#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# 2021-03-31 09:02
import os
import cv2
import tensorflow as tf
import numpy as np
import xml.etree.ElementTree as XML
from functools import partial

from config import *
from box import compute_ground_truth
from transform import Transform

ID_TO_NAME = [
    "aeroplane",
    "bicycle",
    "bird",
    "boat",
    "bottle",
    "bus",
    "car",
    "cat",
    "chair",
    "cow",
    "diningtable",
    "dog",
    "horse",
    "motorbike",
    "person",
    "pottedplant",
    "sheep",
    "sofa",
    "train",
    "tvmonitor",
]
NAME_TO_ID = dict([(v, k) for k, v in enumerate(ID_TO_NAME)])

class VOCDataset:
    def __init__(self, data_dir):
        self.image_dir = os.path.join(data_dir, "JPEGImages")
        self.annotation_dir = os.path.join(data_dir, "Annotations")
        self.ids = [x[:-4] for x in os.listdir(self.image_dir)]
        self.train_ids = self.ids[: len(self.ids) * 3 // 4]
        with open(os.path.join(data_dir, "ImageSets/Main/trainval.txt")) as f:
            self.train_ids = [x.strip() for x in f]
        with open(os.path.join(data_dir, "ImageSets/Main/test.txt")) as f:
            self.test_ids = [x.strip() for x in f]
        self.transform = Transform()

    def _get_image(self, id):
        return cv2.imread(os.path.join(self.image_dir, f"{id}.jpg"), cv2.COLOR_BGR2RGB)

    def _get_annotation(self, id):
        annotation_file = os.path.join(self.annotation_dir, f"{id}.xml")
        objects = XML.parse(annotation_file).findall("object")
        boxes = []
        labels = []
        for obj in objects:
            name = obj.find("name").text.lower().strip()
            box = obj.find("bndbox")
            difficult = int(obj.find("difficult").text)
            if difficult:
                continue
            xmin, ymin, xmax, ymax = (
                int(box.find("xmin").text) - 1,
                int(box.find("ymin").text) - 1,
                int(box.find("xmax").text) - 1,
                int(box.find("ymax").text) - 1,
            )
            boxes.append([xmin, ymin, xmax, ymax])
            labels.append(NAME_TO_ID[name] + 1)
        return np.array(boxes), np.array(labels)

    def _generator(self, subset):
        if subset == "train":
            ids = self.train_ids.copy()
            np.random.shuffle(ids)
        elif subset == "test":
            ids = self.test_ids.copy()
            np.random.shuffle(ids)
        else:
            ids = subset

        for id in ids:
            img = self._get_image(id)
            h, w = img.shape[:2]
            boxes, labels = self._get_annotation(id)
            if len(boxes) == 0:
                continue
            boxes = boxes / np.array([w, h, w, h])
            img, boxes, labels = self.transform(img, boxes, labels)
            confs, locs = compute_ground_truth(boxes, labels)
            yield img, confs, locs

    def get_train_data(self, batch, n_batch):
        return (
            tf.data.Dataset.from_generator(
                partial(self._generator, subset="train"),
                (tf.float32, tf.int64, tf.float32),
            )
            .batch(batch)
            .repeat()
            .take(n_batch)
        )

    def get_test_data(self, batch, n_batch):
        return (
            tf.data.Dataset.from_generator(
                partial(self._generator, subset="test"),
                (tf.float32, tf.int64, tf.float32),
            )
            .batch(batch)
            .repeat()
            .take(n_batch)
        )


if __name__ == "__main__":
    from box import *

    voc = VOCDataset("/home/sunway/download/VOCdevkit/VOC2007")
    _, confs, locs = next(voc._generator(["002477"]))
    image = voc._get_image("002477")
    anchors = gen_anchors()
    corner_anchors = center_to_corner(anchors)
    locs = decode(anchors, locs)

    for i in range(len(confs)):
        if confs[i] == 0:
            continue
        x1, y1, x2, y2 = locs[i]
        xx1, yy1, xx2, yy2 = corner_anchors[i]
        height, width, _ = image.shape
        cv2.rectangle(
            image,
            (int(x1 * width), int(y1 * height)),
            (int(x2 * width), int(y2 * height)),
            (0, 0, 255),
            1,
            1,
        )
        cv2.rectangle(
            image,
            (int(xx1 * width), int(yy1 * height)),
            (int(xx2 * width), int(yy2 * height)),
            (0, 255, 0),
            1,
            1,
        )

    cv2.imshow("", image)
    cv2.waitKey(0)
