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
from summary import Summary

parser = argparse.ArgumentParser()
parser.add_argument("--batch", default=BATCH, type=int)
parser.add_argument("--n-batch", default=N_BATCH, type=int)
parser.add_argument("--epoch", default=EPOCH, type=int)
parser.add_argument(
    "--reset",
    action="store_true",
)
args = parser.parse_args()

dataset = VOCDataset(DATASET)
model = SSDModel(resume=os.path.exists(f"{MODEL_WEIGHTS}.index") and not args.reset)
ssd_loss = SSDLoss()
summary = Summary()
optimizer = tf.keras.optimizers.SGD(
    learning_rate=LR,
    momentum=MOMENTUM,
)


def train_step():
    with tf.GradientTape() as tape:
        confs, locs = model(imgs, training=True)
        conf_loss, loc_loss = ssd_loss(gt_confs, gt_locs, confs, locs)
        l2_loss = [tf.nn.l2_loss(t) for t in model.trainable_variables]
        l2_loss = WEIGHT_DECAY * tf.math.reduce_sum(l2_loss)
        loss = l2_loss + conf_loss + loc_loss
        gradients = tape.gradient(loss, model.trainable_variables)
        optimizer.apply_gradients(zip(gradients, model.trainable_variables))
    return conf_loss, loc_loss


def validation_step(imgs, gt_confs, gt_locs):
    confs, locs = model(imgs, training=False)
    return ssd_loss(gt_confs, gt_locs, confs, locs)


if __name__ == "__main__":
    for epoch in range(args.epoch):
        summary.on_epoch_start(epoch)
        with tqdm(
            dataset.get_train_data(args.batch, args.n_batch),
            unit="batch",
            total=args.n_batch,
        ) as progress:
            progress.set_description(f"Epoch {epoch}")
            for i, (imgs, gt_confs, gt_locs) in enumerate(progress):
                summary.update_train_loss(*train_step())
                if i == (args.n_batch - 1):
                    for j, (imgs, gt_confs, gt_locs) in enumerate(
                        dataset.get_test_data(args.batch, 5)
                    ):
                        summary.update_val_loss(
                            *validation_step(imgs, gt_confs, gt_locs)
                        )
                progress.set_postfix(summary.get_loss())

        summary.on_epoch_end()
        model.save_weights(MODEL_WEIGHTS)
