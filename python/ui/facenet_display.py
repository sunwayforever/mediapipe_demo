#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# 2021-02-24 10:10
import cv2

import util


class FacenetDisplayCallback(object):
    def __init__(self, backend):
        self.backend = backend
        self.image = None
        self.unknown_image = cv2.cvtColor(
            cv2.imread(util.get_resource("../data/unknown_face.png")), cv2.COLOR_BGR2RGB
        )

    def __call__(self, topic, data):
        if topic == b"facenet":
            self.image = data
        if self.image is None:
            self.image = self.unknown_image
        self.backend.update_facenet_image(self.image)
