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
        total_count = gt_confs.shape[1]
        temp_loss = self.temp_cross_entropy(gt_confs, confs)
        pos_index = gt_confs > 0
        pos_count = np.sum(pos_index, axis=1)
        neg_count = pos_count * HARD_NEGATIVE_RATIO
        neg_index = np.argsort(np.argsort(temp_loss, axis=1), axis=1)

        neg_index = neg_index > np.expand_dims(total_count - neg_count, 1)
        # neg_index = np.logical_and(np.logical_not(pos_index), neg_index)
        return pos_index, np.logical_or(neg_index, pos_index)

    def __call__(self, gt_confs, gt_locs, confs, locs):
        pos_index, all_index = self._hard_negative_mining(gt_confs, confs)
        conf_loss = self.cross_entropy(gt_confs[all_index], confs[all_index])
        loc_loss = self.smooth_l1(gt_locs[pos_index], locs[pos_index])
        num_pos = np.sum(pos_index)
        return conf_loss / num_pos + loc_loss / num_pos


if __name__ == "__main__":
    loss = SSDLoss()
    confs = np.random.normal(size=(32, 100, 21)).astype("float32")
    locs = np.random.normal(size=(32, 100, 4)).astype("float32")

    gt_confs = np.random.randint(0, 21, size=(32, 100))
    gt_locs = np.random.normal(size=(32, 100, 4)).astype("float32")

    print(loss(gt_confs, gt_locs, confs, locs))
