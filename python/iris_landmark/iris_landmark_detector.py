#!/usr/bin/env python3
# -*- coding: utf-8 -*-
from cv2 import cv2
import numpy as np

from .config import *
from .iris_points import *
from .eye_estimator import EyeEstimator
from message_broker.transport import Publisher
import util


class IrisLandmarkDetector(object):
    def __init__(self):
        self.publisher = Publisher()
        self.eye_estimator = EyeEstimator()
        self.nn = ""
        if MODEL.endswith(".tflite"):
            import tensorflow as tf

            self.nn = "tflite"
            model_path = util.get_resource(MODEL)
            self.interpreter = tf.lite.Interpreter(model_path=model_path)
            self.interpreter.allocate_tensors()
            self.input_details = self.interpreter.get_input_details()
            self.output_details = self.interpreter.get_output_details()
        elif MODEL.endswith(".onnx"):
            import onnxruntime as onnx

            self.nn = "onnx"
            self.onnx = onnx.InferenceSession(util.get_resource(MODEL))
        else:
            import tensorflow as tf
            from tensorflow import keras

            self.model = keras.models.load_model(
                util.get_resource("../model/iris_landmark")
            ).signatures[  # type: ignore
                "serving_default"
            ]

    def __call__(self, topic, data):
        if topic == b"iris_roi":
            self.detect(data)

    def detect(self, iris_roi):
        eye_surfaces = []
        iris_surfaces = []
        for index, (img, mat) in enumerate(iris_roi):
            # right eye need to be horizontally flipped
            if index == 1:
                img = cv2.flip(img, 1)

            scale_mat = util.get_scale_mat(
                img.shape[1] / IMG_WIDTH,
                img.shape[0] / IMG_HEIGHT,
            )
            mat = mat @ scale_mat

            input_data = cv2.resize(img, (IMG_WIDTH, IMG_HEIGHT)).astype(np.float32)
            input_data = input_data / 255.0
            input_data = np.expand_dims(input_data, axis=0)

            if self.nn == "tflite":
                self.interpreter.set_tensor(self.input_details[0]["index"], input_data)
                self.interpreter.invoke()
                eye_surface = self.interpreter.get_tensor(
                    self.output_details[0]["index"]
                )
                iris_surface = self.interpreter.get_tensor(
                    self.output_details[1]["index"]
                )
            elif self.nn == "onnx":
                eye_surface, iris_surface = self.onnx.run(
                    ["output_eyes_contours_and_brows", "output_iris"],
                    {"input_1": np.transpose(input_data, (0, 3, 1, 2))},
                )
            else:
                output = self.model(tf.convert_to_tensor(input_data))
                eye_surface, iris_surface = (
                    output["output_eyes_contours_and_brows"],
                    output["output_iris"],
                )

            eye_surface, iris_surface = (
                np.reshape(eye_surface, (-1, 3)).copy(),
                np.reshape(iris_surface, (-1, 3)).copy(),
            )

            # right eye was horizontally flipped
            if index == 1:
                eye_surface[:, 0] = IMG_WIDTH - eye_surface[:, 0]
                iris_surface[:, 0] = IMG_WIDTH - iris_surface[:, 0]

            eye_surface = util.restore_coordinates(eye_surface, mat)
            iris_surface = util.restore_coordinates(iris_surface, mat)

            eye_surfaces.append(eye_surface.astype(int))
            iris_surfaces.append(iris_surface.astype(int))

        # ZMQ_PUB: eye_landmark
        self.publisher.pub(b"eye_landmark", eye_surfaces)
        # ZMQ_PUB: iris_landmark
        self.publisher.pub(b"iris_landmark", iris_surfaces)
        # ZMQ_PUB: eye_aspect_ratio
        self.publisher.pub(
            b"eye_aspect_ratio",
            self.eye_estimator.estimate(
                [surface[eye_points] for surface in eye_surfaces]
            ),
        )
