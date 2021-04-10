#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# 2021-04-10 20:13
import tensorflow as tf

from config import *


class Summary(object):
    def __init__(self):
        self.summary_writer = tf.summary.create_file_writer(SUMMARY_DIR)

    def on_epoch_start(self, epoch):
        self.conf_loss = 0
        self.loc_loss = 0
        self.val_loss = 0
        self.batch = 0
        self.val_batch = 0
        self.epoch = epoch

    def on_epoch_end(self):
        with self.summary_writer.as_default():
            for k, v in self.get_loss().items():
                tf.summary.scalar(k, v, step=self.epoch)

    def get_loss(self):
        ret = {}
        if self.batch != 0:
            ret.update(
                {
                    "loss": self.conf_loss / self.batch + self.loc_loss / self.batch,
                    "conf_loss": self.conf_loss / self.batch,
                    "loc_loss": self.loc_loss / self.batch,
                }
            )
        if self.val_batch != 0:
            ret.update(
                {
                    "val_loss": self.val_loss / self.val_batch,
                }
            )
        return ret

    def update_train_loss(self, conf_loss, loc_loss):
        self.conf_loss += conf_loss.numpy()
        self.loc_loss += loc_loss.numpy()
        self.batch += 1

    def update_val_loss(self, conf_loss, loc_loss):
        self.val_loss += conf_loss.numpy() + loc_loss.numpy()
        self.val_batch += 1
