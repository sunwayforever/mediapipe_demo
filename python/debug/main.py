#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# 2021-02-24 17:36
from cv2 import cv2
import numpy as np

from message_broker.transport import Subscriber


class Debugger(object):
    def __init__(self):
        self.face_landmark = None
        self.dlib_landmark = None

    def __call__(self, topic, data):
        if topic == b"face_landmark":
            self.face_landmark = data
        if topic == b"dlib_landmark":
            self.dlib_landmark = data

        self.draw_face_landmark()

    def draw_face_landmark(self):
        if self.face_landmark is None or self.dlib_landmark is None:
            return
        self.image = np.zeros((4000, 4000, 3), np.uint8)
        for i, point in enumerate(self.face_landmark[:, :2]):
            cv2.putText(
                self.image,
                f"{i}",
                tuple(point * 10),
                cv2.FONT_HERSHEY_SIMPLEX,
                1,
                (255, 0, 0),
                1,
                cv2.LINE_AA,
            )
        for i, point in enumerate(self.dlib_landmark[:, :2]):
            cv2.putText(
                self.image,
                f"{i}",
                tuple(point.astype(np.int) * 10),
                cv2.FONT_HERSHEY_SIMPLEX,
                1,
                (0, 255, 0),
                1,
                cv2.LINE_AA,
            )
        cv2.imshow("", self.image)
        cv2.waitKey(0)


def run():
    # ZMQ_SUB:
    Subscriber().sub([b"face_landmark", b"dlib_landmark"], Debugger()).loop()


if __name__ == "__main__":
    run()
