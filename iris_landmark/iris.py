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

sys.path.append(os.path.abspath("../face_landmark"))
from iris_cropper import IrisCropper

IMG_WIDTH = 64
IMG_HEIGHT = 64


class Iris(object):
    def __init__(self):
        model_path = "../model/iris_landmark.tflite"
        # Load TFLite model and allocate tensors.
        self.interpreter = tf.lite.Interpreter(model_path=model_path)
        self.interpreter.allocate_tensors()
        self.input_details = self.interpreter.get_input_details()
        self.output_details = self.interpreter.get_output_details()

        self.input_width = self.input_details[0]["shape"][1]
        self.input_height = self.input_details[0]["shape"][2]
        assert self.input_width == IMG_WIDTH
        assert self.input_height == IMG_HEIGHT

        self.iris_cropper = IrisCropper()

    def __call__(self, img):
        # cropped image, left-upper point of cropped image
        iris = self.iris_cropper(img)
        if iris is None:
            return None, None
        # TODO: left eye only
        img, offset_x, offset_y, offset_x2, offset_y2 = iris[0]
        # cv2.rectangle(img_orig, (offset_x, offset_y), (offset_x2, offset_y2), (255, 0, 0), 2)
        # cv2.imshow("", img_orig)
        # cv2.waitKey(0)
        img_rgb = cv2.cvtColor(img, cv2.COLOR_BGR2RGB)
        input_data = cv2.resize(img_rgb, (self.input_width, self.input_height)).astype(
            np.float32
        )
        input_data = input_data / 255.0
        input_data = np.expand_dims(input_data, axis=0)

        self.interpreter.set_tensor(self.input_details[0]["index"], input_data)
        self.interpreter.invoke()

        # 71x3
        eye_surface = self.interpreter.get_tensor(self.output_details[0]["index"])
        # 5x3
        #           2
        #
        #    3      0       1
        #
        #           4
        iris_surface = self.interpreter.get_tensor(self.output_details[1]["index"])
        eye_surface, iris_surface = (
            np.reshape(eye_surface, (-1, 3)),
            np.reshape(iris_surface, (-1, 3)),
        )

        img_height, img_width = img.shape[0], img.shape[1]
        eye_surface[:, 0] = (eye_surface[:, 0] / IMG_WIDTH * img_width) + offset_x
        eye_surface[:, 1] = (eye_surface[:, 1] / IMG_HEIGHT * img_height) + offset_y
        iris_surface[:, 0] = (iris_surface[:, 0] / IMG_WIDTH * img_width) + offset_x
        iris_surface[:, 1] = (iris_surface[:, 1] / IMG_HEIGHT * img_height) + offset_y
        return eye_surface, iris_surface


def remap(x, lo, hi, scale):
    return (x - lo) / (hi - lo + 1e-6) * scale


eye_landmark_connections = [0, 1, 2, 3, 4, 5, 6, 7, 8, 15, 14, 13, 12, 11, 10, 9]


def annotate_image(img, eye_surface, iris_surface):
    # for x, y, _ in eye_surface:
    #     cv2.circle(img, (x, y), color=(0, 0, 255), radius=1)
    for x, y, _ in iris_surface:
        cv2.circle(img, (x, y), 1, color=(0, 255, 0))
    # eye_surface = eye_surface[:, :2]
    # blank_img = np.zeros(shape=(5120, 5120, 3), dtype=np.uint8)
    # for index, (a, b) in enumerate(eye_surface):
    #     cv2.putText(
    #         blank_img,
    #         f"{index}",
    #         (int(a) * 10, int(b) * 10),
    #         cv2.FONT_HERSHEY_SIMPLEX,
    #         0.6,
    #         (0, 0, 255),
    #         2,
    #     )
    #     cv2.circle(blank_img, (int(a) * 10, int(b) * 10), 1, color=(0, 255, 0))
    # cv2.imshow("", blank_img)
    # while True:
    #     if cv2.waitKey(1) & 0xFF == ord("q"):
    #         break
    for a, b in zip(eye_landmark_connections[::2], eye_landmark_connections[1::2]):
        cv2.line(
            img,
            (eye_surface[a][0], eye_surface[a][1]),
            (eye_surface[b][0], eye_surface[b][1]),
            color=(0, 0, 255),
        )
    return img


def sigmoid(x):
    return 1.0 / (1.0 + np.exp(-x))


def iris_image(img):
    iris = Iris()
    img = cv2.imread(img)
    eye_surface, iris_surface = iris(img)
    img = annotate_image(img, eye_surface, iris_surface)
    cv2.imshow("", img)
    while cv2.waitKey(1) & 0xFF != ord("q"):
        pass
    cv2.destroyAllWindows()


def iris_stream():
    iris = Iris()
    vid = cv2.VideoCapture(0)
    while True:
        succ, img = vid.read()
        if not succ:
            continue
        eye_surface, iris_surface = iris(img)
        if eye_surface is not None:
            img = annotate_image(img, eye_surface, iris_surface)
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
        iris_image(flags.image)
    else:
        iris_stream()
