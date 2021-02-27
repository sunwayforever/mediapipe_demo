#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# 2021-02-24 10:10


class RotationDisplayCallback(object):
    def __init__(self, backend):
        self.rotation = None
        self.backend = backend

    def __call__(self, topic, data):
        if topic == b"rotation":
            self.rotation = data

        if self.rotation:
            self.backend.update_rotation(
                [
                    int(self.rotation[0] * 45),
                    int(self.rotation[1] * 45),
                    int(self.rotation[2] * 45),
                ]
            )
