#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# 2021-02-24 10:10


class ExpressionDisplayCallback(object):
    def __init__(self, backend):
        self.mouth = 0
        self.backend = backend

    def __call__(self, topic, data):

        if topic == b"mouth":
            self.mouth = data

        self.backend.update_mouth(self.mouth > 0.1)
