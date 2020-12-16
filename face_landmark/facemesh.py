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

sys.path.append(os.path.abspath("../face_detection"))
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

        img_rgb = cv2.cvtColor(img, cv2.COLOR_BGR2RGB)
        input_data = cv2.resize(img_rgb, (self.input_width, self.input_height)).astype(
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


face_landmark_connections = [
    # Lips.
    61,
    146,
    146,
    91,
    91,
    181,
    181,
    84,
    84,
    17,
    17,
    314,
    314,
    405,
    405,
    321,
    321,
    375,
    375,
    291,
    61,
    185,
    185,
    40,
    40,
    39,
    39,
    37,
    37,
    0,
    0,
    267,
    267,
    269,
    269,
    270,
    270,
    409,
    409,
    291,
    78,
    95,
    95,
    88,
    88,
    178,
    178,
    87,
    87,
    14,
    14,
    317,
    317,
    402,
    402,
    318,
    318,
    324,
    324,
    308,
    78,
    191,
    191,
    80,
    80,
    81,
    81,
    82,
    82,
    13,
    13,
    312,
    312,
    311,
    311,
    310,
    310,
    415,
    415,
    308,
    # Left eye.
    33,
    7,
    7,
    163,
    163,
    144,
    144,
    145,
    145,
    153,
    153,
    154,
    154,
    155,
    155,
    133,
    33,
    246,
    246,
    161,
    161,
    160,
    160,
    159,
    159,
    158,
    158,
    157,
    157,
    173,
    173,
    133,
    # Left eyebrow.
    46,
    53,
    53,
    52,
    52,
    65,
    65,
    55,
    70,
    63,
    63,
    105,
    105,
    66,
    66,
    107,
    # Right eye.
    263,
    249,
    249,
    390,
    390,
    373,
    373,
    374,
    374,
    380,
    380,
    381,
    381,
    382,
    382,
    362,
    263,
    466,
    466,
    388,
    388,
    387,
    387,
    386,
    386,
    385,
    385,
    384,
    384,
    398,
    398,
    362,
    # Right eyebrow.
    276,
    283,
    283,
    282,
    282,
    295,
    295,
    285,
    300,
    293,
    293,
    334,
    334,
    296,
    296,
    336,
    # Face oval.
    10,
    338,
    338,
    297,
    297,
    332,
    332,
    284,
    284,
    251,
    251,
    389,
    389,
    356,
    356,
    454,
    454,
    323,
    323,
    361,
    361,
    288,
    288,
    397,
    397,
    365,
    365,
    379,
    379,
    378,
    378,
    400,
    400,
    377,
    377,
    152,
    152,
    148,
    148,
    176,
    176,
    149,
    149,
    150,
    150,
    136,
    136,
    172,
    172,
    58,
    58,
    132,
    132,
    93,
    93,
    234,
    234,
    127,
    127,
    162,
    162,
    21,
    21,
    54,
    54,
    103,
    103,
    67,
    67,
    109,
    109,
    10,
]


def annotate_image(img, surface):
    z_min, z_max = surface[:, 2].min(), surface[:, 2].max()
    for x, y, z in surface:
        color = 255 - remap(z, z_min, z_max, 255)
        cv2.circle(img, (x, y), color=(color, color, 0), radius=1, thickness=1)
    for a, b in zip(face_landmark_connections[::2], face_landmark_connections[1::2]):
        cv2.line(
            img,
            (surface[a][0], surface[a][1]),
            (surface[b][0], surface[b][1]),
            color=(0, 255, 0),
        )
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
