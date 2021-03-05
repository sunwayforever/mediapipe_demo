#!/usr/bin/env python3
# -*- coding: utf-8 -*-
from cv2 import cv2
import numpy as np

from .config import *
from .face_points import *
from .pose_estimator import PoseEstimator
from .mouth_estimator import MouthEstimator
from .iris_cropper import IrisCropper
from message_broker.transport import Publisher
import util


class FaceLandmarkDetector(object):
    def __init__(self):
        self.publisher = Publisher()
        self.pose_estimator = PoseEstimator()
        self.mouth_estimator = MouthEstimator()
        self.iris_cropper = IrisCropper()
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
                util.get_resource("../model/face_landmark")
            ).signatures[  # type: ignore
                "serving_default"
            ]

    def __call__(self, topic, data):
        if topic == b"face_roi":
            self.detect(data)

    def detect(self, face_roi):
        face_img, mat = face_roi.image, face_roi.mat
        scale_mat = util.get_scale_mat(
            face_img.shape[1] / IMG_WIDTH,
            face_img.shape[0] / IMG_HEIGHT,
        )
        mat = mat @ scale_mat

        face_img = cv2.resize(face_img, (IMG_WIDTH, IMG_HEIGHT))
        input_data = face_img.astype(np.float32) / 255.0
        input_data = np.expand_dims(input_data, axis=0)

        if self.nn == "tflite":
            self.interpreter.set_tensor(self.input_details[0]["index"], input_data)
            self.interpreter.invoke()
            surface = self.interpreter.get_tensor(self.output_details[0]["index"])
            prob = self.interpreter.get_tensor(self.output_details[1]["index"])
        elif self.nn == "onnx":
            surface, prob = self.onnx.run(
                ["conv2d_20", "conv2d_30"],
                {"input_1": np.transpose(input_data, (0, 3, 1, 2))},
            )
        else:
            output = self.model(tf.convert_to_tensor(input_data))
            surface, prob = output["conv2d_20"], output["conv2d_30"]

        surface, prob = np.reshape(surface, (-1, 3)), np.squeeze(prob)
        surface = np.array(surface)

        if util.sigmoid(prob) < MIN_PROB_THRESH:
            return None

        # ZMQ_PUB: iris_roi
        self.publisher.pub(b"iris_roi", self.iris_cropper.crop(face_img, surface, mat))

        surface = util.restore_coordinates(surface, mat)
        # ZMQ_PUB: face_landmark
        self.publisher.pub(b"face_landmark", surface.astype("float32"))
        # ZMQ_PUB: rotation
        self.publisher.pub(
            b"rotation", self.pose_estimator.estimate(surface[pose_points, :2])
        )
        # ZMQ_PUB: mouth_aspect_ratio
        self.publisher.pub(
            b"mouth_aspect_ratio",
            self.mouth_estimator.estimate(
                surface[
                    mouth_points,
                ]
            ),
        )
