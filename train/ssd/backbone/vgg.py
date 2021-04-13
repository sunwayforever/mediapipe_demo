#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# 2021-04-13 14:58
import tensorflow as tf
from tensorflow.keras import Model
import tensorflow.keras.layers as layers

from config import *
import util

FEATURE_MAP_SIZES = [38, 19, 10, 5, 3, 1]
N_ANCHORS = 8732

class VGG(Model):
    def __init__(self, weights):
        super().__init__()
        self.vgg16_layers = self._get_vgg16_layers(weights)
        self.vgg16_extra_layers = self._get_vgg16_extra_layers()
        self.bn = util.L2Normalization()

    def _get_vgg16_extra_layers(self):
        return [
            layers.MaxPool2D(3, 1, padding="same"),
            layers.Conv2D(1024, 3, padding="same", dilation_rate=6, activation="relu"),
            layers.Conv2D(1024, 1, padding="same", activation="relu"),
        ]

    def _get_vgg16_layers(self, weights):
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

        if weights is not None:
            tmp_input = layers.Input(shape=[None, None, 3])
            tmp_output = tmp_input
            for layer in vgg16_layers:
                tmp_output = layer(tmp_output)
                Model(tmp_input, tmp_output)
                origin_vgg = tf.keras.applications.VGG16(weights=weights)
            for i in range(len(vgg16_layers)):
                vgg16_layers[i].set_weights(
                    origin_vgg.get_layer(index=i + 1).get_weights()
                )

        return vgg16_layers

    def call(self, x):
        for layer in self.vgg16_layers:
            x = layer(x)
            if layer.name == "classifier":
                feature1 = self.bn(x)
        for layer in self.vgg16_extra_layers:
            x = layer(x)
        return x, (feature1, x)

    def get_conf_layers(self):
        return [
            layers.Conv2D(4 * N_CLASSES, kernel_size=3, padding="same"),
            layers.Conv2D(6 * N_CLASSES, kernel_size=3, padding="same"),
            layers.Conv2D(6 * N_CLASSES, kernel_size=3, padding="same"),
            layers.Conv2D(6 * N_CLASSES, kernel_size=3, padding="same"),
            layers.Conv2D(4 * N_CLASSES, kernel_size=3, padding="same"),
            layers.Conv2D(4 * N_CLASSES, kernel_size=1),
        ]

    def get_loc_layers(self):
        return [
            layers.Conv2D(4 * 4, kernel_size=3, padding="same"),
            layers.Conv2D(6 * 4, kernel_size=3, padding="same"),
            layers.Conv2D(6 * 4, kernel_size=3, padding="same"),
            layers.Conv2D(6 * 4, kernel_size=3, padding="same"),
            layers.Conv2D(4 * 4, kernel_size=3, padding="same"),
            layers.Conv2D(4 * 4, kernel_size=1),
        ]

    def get_extra_feature_layers(self):
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
