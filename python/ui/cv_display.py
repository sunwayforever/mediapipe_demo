#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# 2022-03-14 17:10
import cv2


class CVDisplay(object):
    def __init__(self):
        self.image = None
        self.face_box = None
        self.face_landmark = None

    def __call__(self, topic, data):
        if topic == b"image":
            self.image = data
        if topic == b"face_box":
            self.face_box = data
        if topic == b"face_landmark":
            self.face_landmark = data

        self.draw()

    def draw(self):
        if self.image is None:
            return

        if self.face_box is not None:
            cv2.rectangle(
                self.image,
                (self.face_box.xmin, self.face_box.ymin),
                (
                    self.face_box.xmin + self.face_box.width,
                    self.face_box.ymin + self.face_box.height,
                ),
                (255, 0, 0),
                1,
                1,
            )
            for i, point in enumerate(self.face_box.keypoints):
                cv2.circle(self.image, tuple(point), 2, (0, 0, 255), 2)
                # cv2.putText(
                #     self.image,
                #     f"{i}",
                #     tuple(point),
                #     cv2.FONT_HERSHEY_SIMPLEX,
                #     1,
                #     (0, 0, 255),
                #     1,
                #     cv2.LINE_AA,
                # )
        if self.face_landmark is not None:
            for i, point in enumerate(self.face_landmark):
                cv2.circle(self.image, tuple(point), 1, (0, 255, 0), 1)

        cv2.imshow("face", self.image)
        cv2.waitKey(1)
