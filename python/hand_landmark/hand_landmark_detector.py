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


class HandLandmarkDetector(object):
    def __init__(self):
        self.publisher = Publisher()
        self.model = keras.models.load_model(
            util.get_resource("../model/hand_landmark")
        ).signatures["serving_default"]

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

        output = self.model(tf.convert_to_tensor(input_data))

        surface, prob = output["ld_21_3d"], output["output_handflag"]
        surface, prob = np.reshape(surface, (-1, 3)), np.squeeze(prob)
        surface = np.array(surface)
        if prob < MIN_PROB_THRESH:
            return None
        surface = util.restore_coordinates(surface, mat)
        # ZMQ_PUB: hand_landmark
        self.publisher.pub(b"hand_landmark", surface.astype("float32"))
