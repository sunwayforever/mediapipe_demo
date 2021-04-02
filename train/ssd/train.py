#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# 2021-04-01 14:26

import os
import argparse
import tensorflow as tf
from tqdm import tqdm

from model import SSDModel
from voc_dataset import VOCDataset
from model import SSDModel
from loss import SSDLoss
from config import *

parser = argparse.ArgumentParser()
parser.add_argument("--data-dir", default="/data/datasets/VOCdevkit/VOC2007")
parser.add_argument("--batch", default=32, type=int)
parser.add_argument("--n-batch", default=50, type=int)
parser.add_argument("--epoch", default=100, type=int)
parser.add_argument(
    "--reset",
    action="store_true",
)
args = parser.parse_args()


def train(epoch):
    with tqdm(
        dataset.get_train_data(args.batch, args.n_batch),
        unit="batch",
        total=args.n_batch,
    ) as progress:
        progress.set_description(f"Epoch {epoch}")
        avg_loss = 0.0
        for i, (imgs, gt_confs, gt_locs) in enumerate(progress):
            with tf.GradientTape() as tape:
                confs, locs = model(imgs)
                loss = ssd_loss(gt_confs, gt_locs, confs, locs)
                gradients = tape.gradient(loss, model.trainable_variables)
                optimizer.apply_gradients(zip(gradients, model.trainable_variables))
            avg_loss = (avg_loss * i + loss.numpy()) / (i + 1)
            progress.set_postfix(loss=avg_loss)

        for imgs, gt_confs, gt_locs in dataset.get_test_data(args.batch):
            confs, locs = model(imgs)
            val_loss = ssd_loss(gt_confs, gt_locs, confs, locs)
            print(f"validation:{val_loss}")


if __name__ == "__main__":
    dataset = VOCDataset(args.data_dir)
    model = SSDModel(resume=os.path.exists(f"{MODEL_WEIGHTS}.index") and not args.reset)
    ssd_loss = SSDLoss()
    optimizer = tf.keras.optimizers.Adam()
    for epoch in range(args.epoch):
        train(epoch)
        model.save_weights(MODEL_WEIGHTS)
