#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# 2020-12-14 13:32
import numpy as np
import tensorflow as tf
import matplotlib.pyplot as plt
import argparse
import time
import os
import sys
import cv2


class Inference(object):
    def __init__(self):
        self.interpreter = tf.lite.Interpreter("model.tflite")
        self.interpreter.allocate_tensors()
        self.input_details = self.interpreter.get_input_details()
        self.output_details = self.interpreter.get_output_details()

    def __call__(self, input_data):
        self.interpreter.set_tensor(self.input_details[0]["index"], input_data)
        self.interpreter.invoke()
        regressor, classificator = (
            self.interpreter.get_tensor(self.output_details[0]["index"]),
            self.interpreter.get_tensor(self.output_details[1]["index"]),
        )
        return regressor, classificator


def get_image(file):
    data = cv2.imread("image/1.png")
    data = cv2.cvtColor(data, cv2.COLOR_BGR2RGB)
    data = np.expand_dims(data, 0)
    data = (data - 127.5) / 127.5
    return data.astype(np.float32)


if __name__ == "__main__":
    input_data = get_image("image/1.png")
    inference = Inference()
    regressor, classificator = inference(input_data)
    import pdb

    pdb.set_trace()
