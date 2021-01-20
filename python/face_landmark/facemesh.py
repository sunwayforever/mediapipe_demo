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

from .pose_estimator import PoseEstimator
from face_detection.face_cropper import FaceCropper
import util

IMG_WIDTH = 192
IMG_HEIGHT = 192
MIN_PROB_THRESH = 0.5


class Mesh(object):
    def __init__(self):
        model_path = util.get_resource("../model/face_landmark.tflite")
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
        util.benchmark_begin("crop+mesh")
        util.benchmark_begin("crop")
        img, offset_x, offset_y, mat = self.face_cropper(img)
        util.benchmark_end("crop")

        if img is None:
            return None

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
        if sigmoid(prob) < MIN_PROB_THRESH:
            return None
        # test:
        # blank_image = np.zeros((1920, 1920))
        # for index, (x, y, z) in enumerate(surface):
        #     print(x, y, z)
        #     cv2.putText(
        #         blank_image,
        #         f"{index}",
        #         (int(x * 10), int(y * 10)),
        #         cv2.FONT_HERSHEY_SIMPLEX,
        #         0.3,
        #         (255, 255, 255),
        #         1,
        #     )
        # cv2.imshow("", blank_image)
        # while True:
        #     if cv2.waitKey(1) & 0xFF == ord("q"):
        #         break
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
        util.benchmark_end("crop+mesh")
        return surface.astype("float32")


def remap(x, lo, hi, scale):
    return (x - lo) / (hi - lo + 1e-6) * scale


pose_points = [
    # left eye right corner
    173,
    # right eye left corner
    398,
    # nose
    1,
    # mouth left
    43,
    # mouth right
    273,
    # chin
    199,
]

mouth_points = [
    # top
    13,
    # botton
    14,
    # left
    62,
    # right
    292,
]

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


all = []


def annotate_image(img, surface):
    if surface is None:
        return
    z_min, z_max = surface[:, 2].min(), surface[:, 2].max()
    # for x, y, z in surface:
    #     color = 255 - remap(z, z_min, z_max, 255)
    #     cv2.circle(img, (x, y), color=(color, color, 0), radius=1, thickness=1)

    for i in pose_points:
        cv2.circle(img, tuple(surface[i][:2]), color=(0, 255, 0), radius=2, thickness=2)

    for i in mouth_points:
        cv2.circle(img, tuple(surface[i][:2]), color=(0, 0, 255), radius=2, thickness=2)

    # print(surface[face_points])
    # all.append(surface[face_points])
    # print("mean:")
    # print(np.stack(all).mean(axis = 0))

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
    surface = mesh(img)
    annotate_image(img, surface)
    util.show_benchmark(img)
    cv2.imshow("", img)
    while cv2.waitKey(1) & 0xFF != ord("q"):
        pass
    cv2.destroyAllWindows()


def mesh_stream(capture):
    for _ in mesh_generator(capture):
        pass


def mesh_webcam_stream():
    vid = cv2.VideoCapture(0)
    mesh_stream(lambda: cv2.flip(vid.read()[1], 2))


def mesh_inu_stream():
    import inu_stream

    height, width = inu_stream.shape()
    mesh_stream(
        lambda: np.reshape(inu_stream.read(height * width * 3), (height, width, 3))
    )


# the generator is used to interact with blender_mediapipe_operator
def mesh_generator(capture):
    mesh = Mesh()
    img = capture()
    pose_estimator = PoseEstimator((img.shape[0], img.shape[1]))
    while True:
        img = capture()
        surface = mesh(img)
        if surface is not None:
            annotate_image(img, surface)
            util.show_benchmark(img)
            rotation_vector = pose_estimator.estimate(
                img, surface[pose_points, :2], surface[pose_points[2]]
            )
            yield rotation_vector, surface[mouth_points]

        cv2.imshow("", img)
        if cv2.waitKey(1) & 0xFF == ord("q"):
            break

    cv2.destroyAllWindows()


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    group = parser.add_mutually_exclusive_group(required=True)
    group.add_argument("--image", type=str)
    group.add_argument("--webcam", action="store_true")
    group.add_argument("--inu", action="store_true")
    flags = parser.parse_args()
    if flags.image:
        mesh_image(flags.image)
    elif flags.webcam:
        mesh_webcam_stream()
    else:
        mesh_inu_stream()
