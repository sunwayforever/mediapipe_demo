#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# 2021-02-20 12:41
import cv2


class WebCamVideoCapture(object):
    def __init__(self):
        self.vid = cv2.VideoCapture(0)

    def capture(self):
        return cv2.flip(self.vid.read()[1], 2)


class InuVideoCapture(object):
    def __init__(self):
        import inu_stream

        self.height, self.width = inu_stream.shape()

    def capture(self):
        return np.reshape(
            inu_stream.read_bgr_image(self.height * self.width * 3),
            (self.height, self.width, 3),
        )
