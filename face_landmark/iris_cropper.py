#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# 2020-12-15 21:04
from facemesh import Mesh

left_eye_landmarks = [
    33,
    7,
    163,
    144,
    145,
    153,
    154,
    155,
    133,
    33,
    246,
    161,
    160,
    159,
    158,
    157,
    173,
    133,
    # left eyebrow
    46,
    53,
    52,
    65,
    55,
    70,
    63,
    105,
    66,
    107,
]

right_eye_landmarks = [
    263,
    249,
    390,
    373,
    374,
    380,
    381,
    382,
    362,
    263,
    466,
    388,
    387,
    386,
    385,
    384,
    398,
    362,
    # right eyebrow
    276,
    283,
    282,
    295,
    285,
    300,
    293,
    334,
    296,
    336,
]


class IrisCropper(object):
    def __init__(self):
        self.mesh = Mesh()

    def __call__(self, img):
        surface, prob = self.mesh(img)
        return crop_image(img, surface)


def crop_image(img, surface):
    if surface is None:
        return None
    img_height = img.shape[0]
    img_width = img.shape[1]

    left_eye_surface = surface[left_eye_landmarks]
    right_eye_surface = surface[right_eye_landmarks]

    ret = []
    for landmark in (left_eye_landmarks, right_eye_landmarks):
        eye_surface = surface[landmark]
        x1, y1, x2, y2 = (
            eye_surface[:, 0].min(),
            eye_surface[:, 1].min(),
            eye_surface[:, 0].max(),
            eye_surface[:, 1].max(),
        )
        # TODO: ~/source/mediapipe/mediapipe/calculators/image/image_cropping_calculator.cc
        margin_w, margin_h = (x2 - x1) // 2, (y2 - y1) // 2

        ret.append(
            (
                img[
                    max(int(y1 - margin_h), 0) : int(y2 + margin_h),
                    max(int(x1 - margin_w), 0) : int(x2 + margin_w),
                ],
                x1 - margin_w,
                y1 - margin_h,
                x2 + margin_w,
                y2 + margin_h,
            )
        )
    return ret


if __name__ == "__main__":
    import argparse
    import numpy as np
    import cv2

    parser = argparse.ArgumentParser()
    parser.add_argument("file_name", type=str)
    flags = parser.parse_args()

    cropper = IrisCropper()
    img = cv2.imread(flags.file_name)
    for eye_img, x1, y1, x2, y2 in cropper(img):
        cv2.rectangle(img, (int(x1), int(y1)), (int(x2), int(y2)), (255, 0, 0), 2)

    cv2.imshow("", img)
    while True:
        if cv2.waitKey(1) & 0xFF == ord("q"):
            break
