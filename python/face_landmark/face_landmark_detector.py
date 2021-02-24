#!/usr/bin/env python3
# -*- coding: utf-8 -*-
import cv2
import time
import math
import argparse
import numpy as np
import tensorflow as tf
import time
import sys
import os
import zmq
import pickle

from tensorflow import keras
from tensorflow.keras import layers, losses, metrics, models

from .config import *
from .face_points import *
from .pose_estimator import PoseEstimator
from .mouth_estimator import MouthEstimator
from .iris_cropper import IrisCropper
from message_broker.transport import Publisher, Subscriber
import util


class FaceLandmarkDetector(object):
    def __init__(self):
        self.publisher = Publisher()
        self.pose_estimator = PoseEstimator()
        self.mouth_estimator = MouthEstimator()
        self.iris_cropper = IrisCropper()
        if NN == "tflite":
            model_path = util.get_resource("../model/face_landmark.tflite")
            self.interpreter = tf.lite.Interpreter(model_path=model_path)
            self.interpreter.allocate_tensors()
            self.input_details = self.interpreter.get_input_details()
            self.output_details = self.interpreter.get_output_details()
        else:
            self.model = keras.models.load_model(
                util.get_resource("../model/face_landmark")
            ).signatures["serving_default"]

    def __call__(self, topic, data):
        if topic == b"face":
            self.detect(data)

    def _post_detect(self, surface, mat):
        # restore coordinates onto webcam image
        # mat: 3x3 homogeneous
        tmp_surface = surface[:, 0:2]
        tmp_surface = np.concatenate(
            (tmp_surface, np.ones((tmp_surface.shape[0], 1))), axis=1
        )
        tmp_surface = (mat @ tmp_surface.T).T
        surface = np.concatenate((tmp_surface, surface[:, 2:3]), axis=1)
        return surface

    def detect(self, face_roi):
        face_img, mat = face_roi.image, face_roi.mat
        scale_mat = util.get_scale_mat(
            face_img.shape[0] / IMG_WIDTH,
            face_img.shape[1] / IMG_HEIGHT,
        )
        mat = mat @ scale_mat

        face_img = cv2.resize(face_img, (IMG_WIDTH, IMG_HEIGHT))
        input_data = face_img.astype(np.float32) / 255.0
        input_data = np.expand_dims(input_data, axis=0)

        if NN == "tflite":
            self.interpreter.set_tensor(self.input_details[0]["index"], input_data)
            self.interpreter.invoke()
            surface = self.interpreter.get_tensor(self.output_details[0]["index"])
            prob = self.interpreter.get_tensor(self.output_details[1]["index"])
            surface, prob = np.reshape(surface, (-1, 3)), np.squeeze(prob)
        else:
            output = self.model(tf.convert_to_tensor(input_data))
            surface, prob = output["conv2d_20"], output["conv2d_30"]
            surface, prob = np.reshape(surface, (-1, 3)), np.squeeze(prob)
            surface = np.array(surface)

        if util.sigmoid(prob) < MIN_PROB_THRESH:
            return None

        # ZMQ_PUB: iris
        self.publisher.pub(b"iris", self.iris_cropper.crop(face_img, surface, mat))

        surface = self._post_detect(surface, mat)
        # ZMQ_PUB: mesh
        self.publisher.pub(b"mesh", surface.astype("float32"))
        # ZMQ_PUB: rotation
        self.publisher.pub(
            b"rotation", self.pose_estimator.estimate(surface[pose_points, :2])
        )
        # ZMQ_PUB: mouth
        self.publisher.pub(
            b"mouth",
            self.mouth_estimator.estimate(
                surface[
                    mouth_points,
                ]
            ),
        )
