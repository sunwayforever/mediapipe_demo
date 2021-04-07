#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# 2021-04-01 14:26

import os
import argparse
import tensorflow as tf
from tqdm import tqdm

from model import SSDModel
from voc_dataset import VOCDataset
from loss import SSDLoss
from config import *

parser = argparse.ArgumentParser()
parser.add_argument("--data-dir", default="/data/datasets/VOCdevkit/VOC2007")
parser.add_argument("--batch", default=BATCH, type=int)
parser.add_argument("--n-batch", default=N_BATCH, type=int)
parser.add_argument("--epoch", default=EPOCH, type=int)
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
                confs, locs = model(imgs, training=True)
                loss = ssd_loss(gt_confs, gt_locs, confs, locs)
                l2_loss = [tf.nn.l2_loss(t) for t in model.trainable_variables]
                l2_loss = WEIGHT_DECAY * tf.math.reduce_sum(l2_loss)
                loss += l2_loss
                gradients = tape.gradient(loss, model.trainable_variables)
                optimizer.apply_gradients(zip(gradients, model.trainable_variables))
            avg_loss = (avg_loss * i + loss.numpy()) / (i + 1)
            progress.set_postfix(loss=avg_loss)

            if i == (args.n_batch - 1):
                avg_val_loss = 0.0
                for j, (imgs, gt_confs, gt_locs) in enumerate(
                    dataset.get_test_data(args.batch, 5)
                ):
                    confs, locs = model(imgs, training=False)
                    val_loss = ssd_loss(gt_confs, gt_locs, confs, locs)
                    avg_val_loss = (avg_val_loss * j + val_loss.numpy()) / (j + 1)
                progress.set_postfix(loss=avg_loss, val_loss=avg_val_loss)


if __name__ == "__main__":
    dataset = VOCDataset(args.data_dir)
    model = SSDModel(resume=os.path.exists(f"{MODEL_WEIGHTS}.index") and not args.reset)
    ssd_loss = SSDLoss()
    optimizer = tf.keras.optimizers.SGD(learning_rate=LR, momentum=MOMENTUM)
    for epoch in range(args.epoch):
        train(epoch)
        model.save_weights(MODEL_WEIGHTS)
