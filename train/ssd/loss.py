#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# 2021-03-31 17:41
import tensorflow as tf
import numpy as np
from tensorflow.keras.losses import SparseCategoricalCrossentropy, Huber

from config import *


class SSDLoss(object):
    def __init__(self):
        # temp_cross_entropy 是用来比较哪个 box 的 loss 较大, 所以需要指定
        # reduction 为 none 避免 loss 被 reduce 成 scalar
        # conf_layers 没有包含 softmax, 所以 from_logits 需要为 True
        # confs: (B, 8732, 21),
        # gt_confs: (B, 8732), 即 sparse categorical
        # temp_cross_entropy 输出为 (B, 8732)
        # cross_entropy 输出为 (B)
        self.temp_cross_entropy = SparseCategoricalCrossentropy(
            from_logits=True, reduction="none"
        )
        self.cross_entropy = SparseCategoricalCrossentropy(
            from_logits=True, reduction="sum"
        )
        self.smooth_l1 = Huber(reduction="sum")

    def _hard_negative_mining(self, gt_confs, confs):
        # confs: (B, 8732, 21),
        # gt_confs: (B, 8732)
        # conf_layers 没有包含 softmax, 所以 from_logits 需要为 True
        temp_loss = self.temp_cross_entropy(gt_confs, confs)
        pos_idx = gt_confs > 0
        num_pos = tf.reduce_sum(tf.dtypes.cast(pos_idx, tf.int32), axis=1)
        num_neg = num_pos * HARD_NEGATIVE_RATIO

        rank = tf.argsort(temp_loss, axis=1, direction="DESCENDING")
        rank = tf.argsort(rank, axis=1)
        neg_idx = rank < tf.expand_dims(num_neg, 1)

        return pos_idx, tf.math.logical_or(pos_idx, neg_idx)

    def __call__(self, gt_confs, gt_locs, confs, locs):
        pos_index, all_index = self._hard_negative_mining(gt_confs, confs)
        conf_loss = self.cross_entropy(gt_confs[all_index], confs[all_index])
        loc_loss = self.smooth_l1(gt_locs[pos_index], locs[pos_index])
        num_pos = tf.reduce_sum(tf.dtypes.cast(pos_index, tf.float32))
        return conf_loss / num_pos, loc_loss / num_pos


if __name__ == "__main__":
    loss = SSDLoss()
    confs = np.random.normal(size=(32, 100, 21)).astype("float32")
    locs = np.random.normal(size=(32, 100, 4)).astype("float32")

    gt_confs = np.random.randint(0, 21, size=(32, 100))
    gt_locs = np.random.normal(size=(32, 100, 4)).astype("float32")

    print(
        loss(gt_confs, gt_locs, (10 * np.eye(21)).astype("float32")[gt_confs], gt_locs)
    )
    # print(loss(gt_confs, gt_locs, confs, locs))
