#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# 2021-04-13 15:04
import tensorflow as tf
from tensorflow.keras import Model, Sequential
import tensorflow.keras.layers as layers

from config import *
import util

FEATURE_MAP_SIZES = [19, 10, 5, 3, 2, 1]
N_ANCHORS = 2268

class MobileNet(Model):
    def __init__(self, weights):
        super().__init__()
        self.bn = util.L2Normalization()

        input = tf.keras.Input((IMAGE_SIZE, IMAGE_SIZE, 3))
        mobile_net = tf.keras.applications.MobileNetV2(
            include_top=False,
            weights=weights,
            input_tensor=input,
        )

        self.model = tf.keras.Model(
            inputs=input,
            outputs=[
                mobile_net.get_layer("out_relu").output,
                mobile_net.get_layer("block_13_expand_relu").output,
            ],
        )

    def call(self, x):
        x, feature1 = self.model(x)
        return x, (self.bn(feature1), x)

    def get_conf_layers(self):
        return [
            layers.SeparableConv2D(4 * N_CLASSES, kernel_size=3, padding="same"),
            layers.SeparableConv2D(6 * N_CLASSES, kernel_size=3, padding="same"),
            layers.SeparableConv2D(6 * N_CLASSES, kernel_size=3, padding="same"),
            layers.SeparableConv2D(6 * N_CLASSES, kernel_size=3, padding="same"),
            layers.SeparableConv2D(4 * N_CLASSES, kernel_size=3, padding="same"),
            layers.Conv2D(4 * N_CLASSES, kernel_size=1),
        ]

    def get_loc_layers(self):
        return [
            layers.SeparableConv2D(4 * 4, kernel_size=3, padding="same"),
            layers.SeparableConv2D(6 * 4, kernel_size=3, padding="same"),
            layers.SeparableConv2D(6 * 4, kernel_size=3, padding="same"),
            layers.SeparableConv2D(6 * 4, kernel_size=3, padding="same"),
            layers.SeparableConv2D(4 * 4, kernel_size=3, padding="same"),
            layers.Conv2D(4 * 4, kernel_size=1),
        ]

    # 5, 3, 2, 1
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
                    layers.Conv2D(256, 3, strides=2, padding="same", activation="relu"),
                ]
            ),
            Sequential(
                [
                    layers.Conv2D(128, 1, activation="relu"),
                    layers.Conv2D(256, 3, strides=3, padding="same", activation="relu"),
                ]
            ),
        ]
