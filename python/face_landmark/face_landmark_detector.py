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
from message_broker.transport import Publisher, Subscriber
import util


class FaceLandmarkDetector(object):
    def __init__(self):
        self.publisher = Publisher()
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

    def detect(self, face_roi):
        img, offset_x, offset_y, mat = (
            face_roi.image,
            face_roi.offset_x,
            face_roi.offset_y,
            face_roi.rotation,
        )

        img_rgb = cv2.cvtColor(img, cv2.COLOR_BGR2RGB)
        input_data = cv2.resize(img_rgb, (IMG_WIDTH, IMG_HEIGHT)).astype(np.float32)
        input_data = input_data / 255.0
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

        img_height, img_width = img.shape[0], img.shape[1]
        # scale
        surface[:, 0] = surface[:, 0] / IMG_WIDTH * img_width
        surface[:, 1] = surface[:, 1] / IMG_HEIGHT * img_height
        # rotate
        tmp_surface = surface[:, 0:2]
        tmp_surface = np.concatenate(
            (tmp_surface, np.ones((tmp_surface.shape[0], 1))), axis=1
        )
        tmp_surface = (mat @ tmp_surface.T).T
        surface = np.concatenate((tmp_surface, surface[:, 2:3]), axis=1)
        # shift
        surface[:, 0] += offset_x
        surface[:, 1] += offset_y
        # ZMQ_PUB: mesh
        self.publisher.pub(b"mesh", surface.astype("float32"))
