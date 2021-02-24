#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# 2020-12-15 21:04
from .face_points import *


class IrisCropper(object):
    def __init__(self):
        pass

    def crop(self, img, surface):
        img_height = img.shape[0]
        img_width = img.shape[1]

        left_eye_surface = surface[left_eye_points]
        right_eye_surface = surface[right_eye_points]

        ret = []
        for landmark in (left_eye_points, right_eye_points):
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
                )
            )
        return ret
