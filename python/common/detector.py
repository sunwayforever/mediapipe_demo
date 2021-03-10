#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# 2021-03-09 21:18
import tensorflow as tf
from tensorflow import keras
import onnxruntime as onnx
import numpy as np

from common import util


class Detector(object):
    def __init__(self, model, output_fmt):
        if model.endswith(".tflite"):
            self.nn = "tflite"
            self.interpreter = tf.lite.Interpreter(model_path=model)
            self.interpreter.allocate_tensors()
            self.input_details = self.interpreter.get_input_details()
            self.output_details = self.interpreter.get_output_details()
        elif model.endswith(".onnx"):
            self.nn = "onnx"
            self.onnx = onnx.InferenceSession(model)
        else:
            self.nn = "tf"
            self.model = keras.models.load_model(model).signatures[  # type:ignore
                "serving_default"
            ]
        self.output_fmt = output_fmt[self.nn]

    def invoke(self, input_data):
        if self.nn == "tflite":
            self.interpreter.set_tensor(self.input_details[0]["index"], input_data)
            self.interpreter.invoke()
            output = []
            for i in self.output_fmt:
                output.append(
                    self.interpreter.get_tensor(self.output_details[i]["index"])
                )
            return output
        elif self.nn == "onnx":
            output = self.onnx.run(
                self.output_fmt,
                {"input": np.transpose(input_data, (0, 3, 1, 2))},
            )
            return output
        else:
            output = self.model(
                tf.convert_to_tensor(np.transpose(input_data, (0, 3, 1, 2)))
            )
            return [output[i] for i in self.output_fmt]
