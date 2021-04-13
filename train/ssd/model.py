#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# 2021-03-31 14:05
import tensorflow as tf
import numpy as np
from tensorflow.keras import Model, Sequential
from tensorflow.keras.applications import VGG16
import tensorflow.keras.layers as layers
from tensorflow.keras import backend as K

from backbone import *
from config import *


class SSDModel(Model):
    def __init__(self, resume=False):
        super().__init__()
        self.backbone = Backbone(None if resume else "imagenet")
        self.extra_feature_layers = self.backbone.get_extra_feature_layers()
        self.conf_layers = self.backbone.get_conf_layers()
        self.loc_layers = self.backbone.get_loc_layers()
        self.dropout = [layers.Dropout(0.2) for _ in range(6)]
        if resume:
            print(f"load weights from {MODEL_WEIGHTS}")
            self.load_weights(MODEL_WEIGHTS)

    def _reset_classifier(self):
        self.confs = []
        self.locs = []
        self.index = 0

    def _classify(self, x):
        if USE_DROPOUT:
            x = self.dropout[self.index](x)
        conf = self.conf_layers[self.index](x)
        conf = tf.reshape(conf, [conf.shape[0], -1, N_CLASSES])
        loc = self.loc_layers[self.index](x)
        loc = tf.reshape(loc, [loc.shape[0], -1, 4])

        self.confs.append(conf)
        self.locs.append(loc)
        self.index += 1

    def call(self, x, training=True):
        K.set_learning_phase(training)
        self._reset_classifier()
        # vgg16
        x, features = self.backbone(x)
        for f in features:
            self._classify(f)
        # extra layer layers
        for layer in self.extra_feature_layers:
            x = layer(x)
            self._classify(x)

        return tf.concat(self.confs, axis=1), tf.concat(self.locs, axis=1)


if __name__ == "__main__":
    import numpy as np

    ssd = SSDModel()
    input = np.random.normal(size=(1, 300, 300, 3))
    confs, locs = ssd(input)
    print(confs.shape)
    print(locs.shape)
