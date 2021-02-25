#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# 2021-02-24 10:10


class ExpressionDisplayCallback(object):
    def __init__(self, backend):
        self.mouth_aspect_ratio = 0
        self.eye_aspect_ratios = [0, 0]
        self.backend = backend

    def __call__(self, topic, data):

        if topic == b"mouth_aspect_ratio":
            self.mouth_aspect_ratio = data

        if topic == b"eye_aspect_ratio":
            self.eye_aspect_ratios = data

        self.backend.update_mouth(self.mouth_aspect_ratio > 0.1)
        self.backend.update_eye(*[x < 0.2 for x in self.eye_aspect_ratios])
