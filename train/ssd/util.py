#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# 2021-04-10 18:02
import tensorflow as tf


def convert_to_tflite(model, output):
    converter = tf.lite.TFLiteConverter.from_keras_model(model)
    tflite_model = converter.convert()

    with open(output, "wb") as f:
        f.write(tflite_model)
        print("converted model saved to %s, size: %d " % (output, len(tflite_model)))
