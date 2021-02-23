#!/usr/bin/env python3
# -*- coding: utf-8 -*-
from .config import *
from .box_detector import BoxDetector
from .face_cropper import FaceCropper
from message_broker.transport import Publisher


class FaceDetector(object):
    def __init__(self, video_capture):
        self.video_capture = video_capture
        sample_image = self.video_capture.capture()
        self.image_height, self.image_width = (
            sample_image.shape[0],
            sample_image.shape[1],
        )
        self.box_detector = BoxDetector()
        self.face_cropper = FaceCropper()
        self.publisher = Publisher()

    def detect(self):
        while True:
            img = self.video_capture.capture()
            print("capture")
            # ZMQ_PUB: image
            self.publisher.pub(b"image", img)
            boxes = self.box_detector.detect(img)
            if not boxes:
                continue
            # NOTE: only one face is detected
            box = boxes[0]
            # ZMQ_PUB: box
            self.publisher.pub(b"box", box)

            face = self.face_cropper.crop(img, box)
            # ZMQ_PUB: face
            self.publisher.pub(b"face", face)
