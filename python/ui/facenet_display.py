#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# 2021-02-24 10:10
from cv2 import cv2

from common import util


class FacenetDisplay(object):
    def __init__(self, backend):
        self.backend = backend
        self.image = None
        self.unknown_image = cv2.cvtColor(
            cv2.imread(util.get_resource("../data/unknown_face.png")), cv2.COLOR_BGR2RGB
        )

    def reset(self):
        self.image = None

    def __call__(self, topic, data):
        if topic == b"facenet":
            self.image = data

        if topic == b"face_reset":
            self.reset()

        enrolled = self.image is not None
        if self.image is None:
            self.image = self.unknown_image

        self.backend.update_facenet_image(self.image, enrolled)
