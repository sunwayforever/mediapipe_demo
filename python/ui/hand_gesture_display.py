#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# 2021-02-24 10:10
import cv2

from common import util


class HandGestureDisplay(object):
    def __init__(self, backend):
        self.hand_gesture = None
        self.backend = backend
        self.unknown_image = cv2.cvtColor(
            cv2.imread(util.get_resource("../data/unknown_gesture.png")),
            cv2.COLOR_BGR2RGB,
        )
        self.mapping = {}
        for i in ["one", "two", "three", "four", "five", "thumb"]:
            self.mapping[i] = cv2.cvtColor(
                cv2.imread(util.get_resource(f"../data/{i}.png")),
                cv2.COLOR_BGR2RGB,
            )

    def __call__(self, topic, data):
        if topic == b"hand_gesture":
            self.hand_gesture = data
        if topic == b"palm_reset":
            self.hand_gesture = None

        self.backend.update_hand_gesture(
            self.mapping.get(self.hand_gesture, self.unknown_image)
        )
