#!/usr/bin/env python3
# -*- coding: utf-8 -*-
import cv2
import time
import math
import argparse
import numpy as np
import tensorflow as tf
import matplotlib.pyplot as plt
import time
import sys
import os

sys.path.append(os.path.abspath("../face-detection"))
from face_cropper import FaceCropper

IMG_WIDTH = 192
IMG_HEIGHT = 192
MIN_PROB_THRESH = 0.8


class Mesh(object):
    def __init__(self):
        model_path = "../model/face_landmark.tflite"
        # Load TFLite model and allocate tensors.
        self.interpreter = tf.lite.Interpreter(model_path=model_path)
        self.interpreter.allocate_tensors()
        self.input_details = self.interpreter.get_input_details()
        self.output_details = self.interpreter.get_output_details()

        self.input_width = self.input_details[0]["shape"][1]
        self.input_height = self.input_details[0]["shape"][2]
        assert self.input_width == IMG_WIDTH
        assert self.input_height == IMG_HEIGHT

        self.face_cropper = FaceCropper()

    def __call__(self, img):
        # cropped image, left-upper point of cropped image
        img, offset_x, offset_y = self.face_cropper(img)
        if img is None:
            return None, 0.0

        input_data = cv2.resize(img, (self.input_width, self.input_height)).astype(
            np.float32
        )
        input_data = input_data / 255.0
        input_data = np.expand_dims(input_data, axis=0)

        self.interpreter.set_tensor(self.input_details[0]["index"], input_data)
        self.interpreter.invoke()

        surface = self.interpreter.get_tensor(self.output_details[0]["index"])
        prob = self.interpreter.get_tensor(self.output_details[1]["index"])
        surface, prob = np.reshape(surface, (-1, 3)), np.squeeze(prob)

        img_height, img_width = img.shape[0], img.shape[1]
        surface[:, 0] = (surface[:, 0] / IMG_WIDTH * img_width) + offset_x
        surface[:, 1] = (surface[:, 1] / IMG_HEIGHT * img_height) + offset_y
        return surface, sigmoid(prob)


def annotate_image(img, surface):
    for x, y, _ in surface:
        cv2.circle(img, (x, y), color=(0, 0, 255), radius=1, thickness=2)
    return img


def sigmoid(x):
    return 1.0 / (1.0 + np.exp(-x))


def mesh_image(img):
    mesh = Mesh()
    img = cv2.imread(img)
    surface, prob = mesh(img)
    if prob < MIN_PROB_THRESH:
        return
    img = annotate_image(img, surface)
    cv2.imshow("", img)
    while cv2.waitKey(1) & 0xFF != ord("q"):
        pass
    cv2.destroyAllWindows()


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    group = parser.add_mutually_exclusive_group(required=True)
    group.add_argument("--stream", action="store_true")
    group.add_argument("--image", type=str)
    flags = parser.parse_args()
    if flags.image:
        mesh_image(flags.image)
    else:
        mesh_stream()