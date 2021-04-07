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
from box import compute_ground_truth, gen_anchors, decode
from augment import Augment


class VOCDataset:
    def __init__(self, data_dir):
        self.image_dir = os.path.join(data_dir, "JPEGImages")
        self.annotation_dir = os.path.join(data_dir, "Annotations")
        self.ids = [x[:-4] for x in os.listdir(self.image_dir)]
        self.train_ids = self.ids[: len(self.ids) * 3 // 4]
        self.test_ids = self.ids[len(self.ids) * 3 // 4 :]
        self.augment = Augment()

        self.id_to_name = [
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
        self.name_to_id = dict([(v, k) for k, v in enumerate(self.id_to_name)])

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
            xmin, ymin, xmax, ymax = (
                int(box.find("xmin").text) - 1,
                int(box.find("ymin").text) - 1,
                int(box.find("xmax").text) - 1,
                int(box.find("ymax").text) - 1,
            )
            boxes.append([xmin, ymin, xmax, ymax])
            labels.append(self.name_to_id[name] + 1)
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
            img, boxes, labels = self.augment(img, boxes, labels)

            boxes = boxes / np.array([w, h, w, h])
            img = cv2.resize(img, (IMAGE_SIZE, IMAGE_SIZE))
            img = (img - 127.5) / 127.5
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
    voc = VOCDataset("/home/sunway/download/VOCdevkit/VOC2007")
    image, confs, locs = next(voc._generator(["002477"]))
    anchors = gen_anchors()
    locs = decode(anchors, locs)

    for i in range(len(confs)):
        if confs[i] == 0:
            continue
        x1, y1, x2, y2 = locs[i]
        height, width, _ = image.shape
        cv2.rectangle(
            image,
            (int(x1 * width), int(y1 * height)),
            (int(x2 * width), int(y2 * height)),
            (0, 0, 255),
            1,
            1,
        )

    cv2.imshow("", image)
    cv2.waitKey(0)
