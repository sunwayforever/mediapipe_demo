#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# 2021-04-01 14:26

import argparse

from model import SSDModel
import tensorflow as tf

from voc_dataset import VOCDataset
from model import SSDModel
from loss import SSDLoss

parser = argparse.ArgumentParser()
parser.add_argument("--data-dir", default="/data/datasets/VOCdevkit/VOC2007")
parser.add_argument("--batch", default=32, type=int)
parser.add_argument("--epoch", default=10, type=int)
args = parser.parse_args()


def train():
    avg_loss = 0.0
    for i, (imgs, gt_confs, gt_locs) in enumerate(dataset.get_train_data(args.batch)):
        with tf.GradientTape() as tape:
            confs, locs = model(imgs)
            loss = ssd_loss(gt_confs, gt_locs, confs, locs)
            gradients = tape.gradient(loss, model.trainable_variables)
            optimizer.apply_gradients(zip(gradients, model.trainable_variables))

        avg_loss = (avg_loss * i + loss.numpy()) / (i + 1)
        if (i + 1) % 10 == 0:
            print(f"traing: {avg_loss}")


def validate():
    for imgs, gt_confs, gt_locs in dataset.get_test_data(args.batch * 5):
        confs, locs = model(imgs)
        loss = ssd_loss(gt_confs, gt_locs, confs, locs)
        print(f"validation: {loss.numpy()}")


if __name__ == "__main__":
    dataset = VOCDataset(args.data_dir)
    model = SSDModel()
    ssd_loss = SSDLoss()
    optimizer = tf.keras.optimizers.Adam()
    for epoch in range(args.epoch):
        train()
        validate()
