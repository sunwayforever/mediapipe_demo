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
MIN_PROB_THRESH = 0.5


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


def remap(x, lo, hi, scale):
    return (x - lo) / (hi - lo + 1e-6) * scale


def annotate_image(img, surface):
    z_min, z_max = surface[:, 2].min(), surface[:, 2].max()
    for x, y, z in surface:
        color = 255 - remap(z, z_min, z_max, 255)
        cv2.circle(img, (x, y), color=(color, color, color), radius=1, thickness=1)
    return img


def sigmoid(x):
    return 1.0 / (1.0 + np.exp(-x))


def mesh_image(img):
    mesh = Mesh()
    img = cv2.imread(img)
    surface, prob = mesh(img)
    if prob >= MIN_PROB_THRESH:
        img = annotate_image(img, surface)
    cv2.imshow("", img)
    while cv2.waitKey(1) & 0xFF != ord("q"):
        pass
    cv2.destroyAllWindows()


def mesh_stream():
    mesh = Mesh()
    vid = cv2.VideoCapture(0)
    while True:
        succ, img = vid.read()
        if not succ:
            continue
        surface, prob = mesh(img)
        if prob >= MIN_PROB_THRESH:
            img = annotate_image(img, surface)
        cv2.imshow("", img)
        if cv2.waitKey(1) & 0xFF == ord("q"):
            break

    vid.release()
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
