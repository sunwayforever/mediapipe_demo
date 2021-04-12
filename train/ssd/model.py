#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# 2021-03-31 14:05
import tensorflow as tf
import numpy as np
from tensorflow.keras import Model, Sequential
from tensorflow.keras.applications import VGG16
import tensorflow.keras.layers as layers
from tensorflow.keras import backend as K

from config import *


class L2Normalization(layers.Layer):
    def __init__(self, gamma_init=2, **kwargs):
        self.gamma_init = gamma_init
        super().__init__(**kwargs)

    def build(self, input_shape):
        self.input_spec = [layers.InputSpec(shape=input_shape)]
        gamma = self.gamma_init * np.ones((input_shape[3],))
        self.gamma = K.variable(gamma, name="{}_gamma".format(self.name))
        self.trainable_weights.append(self.gamma)
        super().build(input_shape)

    def call(self, x, mask=None):
        outputs = K.l2_normalize(x, 3)
        return outputs * self.gamma


def get_vgg16_layers(resume):
    # get vgg16 layers, but ignore layers after `conv 5-3`
    # vgg = VGG16(weights="imagenet", include_top=False)
    # vgg16_conv4 = tf.keras.Model(vgg.inputs[0], vgg.get_layer("block5_conv3").tmp_output)
    vgg16_layers = [
        layers.Conv2D(64, 3, padding="same", activation="relu"),
        layers.Conv2D(64, 3, padding="same", activation="relu"),
        layers.MaxPool2D(2, 2, padding="same"),
        layers.Conv2D(128, 3, padding="same", activation="relu"),
        layers.Conv2D(128, 3, padding="same", activation="relu"),
        layers.MaxPool2D(2, 2, padding="same"),
        layers.Conv2D(256, 3, padding="same", activation="relu"),
        layers.Conv2D(256, 3, padding="same", activation="relu"),
        layers.Conv2D(256, 3, padding="same", activation="relu"),
        layers.MaxPool2D(2, 2, padding="same"),
        layers.Conv2D(512, 3, padding="same", activation="relu"),
        layers.Conv2D(512, 3, padding="same", activation="relu"),
        layers.Conv2D(512, 3, padding="same", activation="relu", name="classifier"),
        layers.MaxPool2D(2, 2, padding="same"),
        layers.Conv2D(512, 3, padding="same", activation="relu"),
        layers.Conv2D(512, 3, padding="same", activation="relu"),
        layers.Conv2D(512, 3, padding="same", activation="relu"),
    ]

    if not resume:
        tmp_input = layers.Input(shape=[None, None, 3])
        tmp_output = tmp_input
        for layer in vgg16_layers:
            tmp_output = layer(tmp_output)
        Model(tmp_input, tmp_output)
        origin_vgg = VGG16(weights="imagenet")
        for i in range(len(vgg16_layers)):
            vgg16_layers[i].set_weights(origin_vgg.get_layer(index=i + 1).get_weights())

    return vgg16_layers


def get_vgg16_extra_layers():
    return [
        layers.MaxPool2D(3, 1, padding="same"),
        layers.Conv2D(1024, 3, padding="same", dilation_rate=6, activation="relu"),
        layers.Conv2D(1024, 1, padding="same", activation="relu"),
    ]


def get_extra_feature_layers():
    return [
        Sequential(
            [
                layers.Conv2D(256, 1, activation="relu"),
                layers.Conv2D(512, 3, strides=2, padding="same", activation="relu"),
            ]
        ),
        Sequential(
            [
                layers.Conv2D(128, 1, activation="relu"),
                layers.Conv2D(256, 3, strides=2, padding="same", activation="relu"),
            ]
        ),
        Sequential(
            [
                layers.Conv2D(128, 1, activation="relu"),
                layers.Conv2D(256, 3, padding="valid", activation="relu"),
            ]
        ),
        Sequential(
            [
                layers.Conv2D(128, 1, activation="relu"),
                layers.Conv2D(256, 3, padding="valid", activation="relu"),
            ]
        ),
    ]


def get_conf_layers():
    #
    return [
        layers.Conv2D(4 * N_CLASSES, kernel_size=3, padding="same"),
        layers.Conv2D(6 * N_CLASSES, kernel_size=3, padding="same"),
        layers.Conv2D(6 * N_CLASSES, kernel_size=3, padding="same"),
        layers.Conv2D(6 * N_CLASSES, kernel_size=3, padding="same"),
        layers.Conv2D(4 * N_CLASSES, kernel_size=3, padding="same"),
        layers.Conv2D(4 * N_CLASSES, kernel_size=1),
    ]


def get_loc_layers():
    return [
        layers.Conv2D(4 * 4, kernel_size=3, padding="same"),
        layers.Conv2D(6 * 4, kernel_size=3, padding="same"),
        layers.Conv2D(6 * 4, kernel_size=3, padding="same"),
        layers.Conv2D(6 * 4, kernel_size=3, padding="same"),
        layers.Conv2D(4 * 4, kernel_size=3, padding="same"),
        layers.Conv2D(4 * 4, kernel_size=1),
    ]


class SSDModel(Model):
    def __init__(self, resume=False):
        super().__init__()
        self.vgg16_layers = get_vgg16_layers(resume)
        self.vgg16_extra_layers = get_vgg16_extra_layers()
        self.extra_feature_layers = get_extra_feature_layers()
        self.conf_layers = get_conf_layers()
        self.bn = L2Normalization()
        self.loc_layers = get_loc_layers()
        if resume:
            print(f"load weights from {MODEL_WEIGHTS}")
            self.load_weights(MODEL_WEIGHTS)

    def reset_classifier(self):
        self.confs = []
        self.locs = []
        self.index = 0

    def classify(self, x):
        conf = self.conf_layers[self.index](x)
        conf = tf.reshape(conf, [conf.shape[0], -1, N_CLASSES])
        loc = self.loc_layers[self.index](x)
        loc = tf.reshape(loc, [loc.shape[0], -1, 4])

        self.confs.append(conf)
        self.locs.append(loc)
        self.index += 1

    def call(self, x, training=True):
        K.set_learning_phase(training)
        self.reset_classifier()
        # vgg16
        for layer in self.vgg16_layers:
            x = layer(x)
            if layer.name == "classifier":
                self.classify(self.bn(x))
        # vgg16 extra layers
        for layer in self.vgg16_extra_layers:
            x = layer(x)
        self.classify(x)
        # extra layer layers
        for layer in self.extra_feature_layers:
            x = layer(x)
            self.classify(x)

        return tf.concat(self.confs, axis=1), tf.concat(self.locs, axis=1)


if __name__ == "__main__":
    import numpy as np

    ssd = SSDModel()
    input = np.random.normal(size=(1, 300, 300, 3))
    confs, locs = ssd(input)
    print(confs.shape)
    print(locs.shape)