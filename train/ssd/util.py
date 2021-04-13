#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# 2021-04-10 18:02
import tensorflow as tf
import numpy as np
import tensorflow.keras.layers as layers
from tensorflow.keras import backend as K


def convert_to_tflite(model, output):
    converter = tf.lite.TFLiteConverter.from_keras_model(model)
    tflite_model = converter.convert()

    with open(output, "wb") as f:
        f.write(tflite_model)
        print("converted model saved to %s, size: %d " % (output, len(tflite_model)))


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
