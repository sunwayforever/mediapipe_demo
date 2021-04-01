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
from box import compute_groud_truth


class VOCDataset:
    def __init__(self, data_dir):
        self.image_dir = os.path.join(data_dir, "JPEGImages")
        self.annotation_dir = os.path.join(data_dir, "Annotations")
        self.ids = [x[:-4] for x in os.listdir(self.image_dir)]
        self.train_ids = self.ids[: len(self.ids) * 3 // 4]
        self.test_ids = self.ids[len(self.ids) * 3 // 4 :]
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
                int(box.find("xmin").text),
                int(box.find("ymin").text),
                int(box.find("xmax").text),
                int(box.find("ymax").text),
            )
            boxes.append([xmin, ymin, xmax, ymax])
            labels.append(self.name_to_id[name] + 1)
        return np.array(boxes), np.array(labels)

    def _generator(self, subset):
        if subset == "train":
            ids = self.train_ids
        else:
            ids = self.test_ids
        for id in ids:
            img = self._get_image(id)
            h, w = img.shape[:2]
            boxes, labels = self._get_annotation(id)
            boxes = boxes / np.array([w, h, w, h])
            img = cv2.resize(img, (IMAGE_SIZE, IMAGE_SIZE))
            img = (img - 127.5) / 127.5
            confs, locs = compute_groud_truth(boxes, labels)
            yield img, confs, locs

    def get_train_data(self, batch):
        return (
            tf.data.Dataset.from_generator(
                partial(self._generator, subset="train"),
                (tf.float32, tf.int64, tf.float32),
            )
            .shuffle(batch)
            .batch(batch)
        )

    def get_test_data(self, batch):
        return tf.data.Dataset.from_generator(
            partial(self._generator, subset="test"),
            (tf.float32, tf.int64, tf.float32),
        ).batch(batch)


if __name__ == "__main__":
    voc = VOCDataset("/home/sunway/download/VOCdevkit/VOC2007")
    print(voc._get_annotation("004954"))

    data = voc.get_test_data(batch=5)
    for x in data:
        print(x)
