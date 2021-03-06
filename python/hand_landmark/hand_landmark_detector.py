#!/usr/bin/env python3
# -*- coding: utf-8 -*-
from cv2 import cv2
import numpy as np
import tensorflow as tf
from tensorflow import keras
import onnxruntime as onnx

from .config import *
from .hand_gesture_estimator import HandGestureEstimator
from message_broker.transport import Publisher
import util


class HandLandmarkDetector(object):
    def __init__(self):
        self.publisher = Publisher()
        self.gesture_estimator = HandGestureEstimator()
        self.nn = ""
        if MODEL.endswith(".tflite"):
            self.nn = "tflite"
            model_path = util.get_resource(MODEL)
            self.interpreter = tf.lite.Interpreter(model_path=model_path)
            self.interpreter.allocate_tensors()
            self.input_details = self.interpreter.get_input_details()
            self.output_details = self.interpreter.get_output_details()
        elif MODEL.endswith(".onnx"):
            self.nn = "onnx"
            self.onnx = onnx.InferenceSession(util.get_resource(MODEL))
        else:
            self.model = keras.models.load_model(
                util.get_resource(MODEL)
            ).signatures[  # type:ignore
                "serving_default"
            ]

    def __call__(self, topic, data):
        if topic == b"palm_roi":
            self.detect(data)

    def detect(self, palm_roi):
        palm_img, mat = palm_roi.image, palm_roi.mat
        scale_mat = util.get_scale_mat(
            palm_img.shape[1] / IMG_WIDTH,
            palm_img.shape[0] / IMG_HEIGHT,
        )
        mat = mat @ scale_mat

        palm_img = cv2.resize(palm_img, (IMG_WIDTH, IMG_HEIGHT))
        input_data = palm_img.astype(np.float32) / 255.0
        input_data = np.expand_dims(input_data, axis=0)

        if self.nn == "tflite":
            self.interpreter.set_tensor(self.input_details[0]["index"], input_data)
            self.interpreter.invoke()
            surface = self.interpreter.get_tensor(self.output_details[0]["index"])
            prob = self.interpreter.get_tensor(self.output_details[1]["index"])
        elif self.nn == "onnx":
            surface, prob = self.onnx.run(
                ["Identity", "Identity_1"],
                {"input_1": np.transpose(input_data, (0, 3, 1, 2))},
            )
        else:
            output = self.model(
                tf.convert_to_tensor(np.transpose(input_data, (0, 3, 1, 2)))
            )
            surface, prob = output["output_0"], output["output_1"]

        surface, prob = np.reshape(surface, (-1, 3)), np.squeeze(prob)
        surface = np.array(surface)
        if prob < MIN_PROB_THRESH:
            return None
        surface = util.restore_coordinates(surface, mat)
        # ZMQ_PUB: hand_landmark
        self.publisher.pub(b"hand_landmark", surface.astype("float32"))
        # ZMQ_PUB: hand_gesture
        gesture = self.gesture_estimator.estimate(surface)
        self.publisher.pub(b"hand_gesture", gesture)
