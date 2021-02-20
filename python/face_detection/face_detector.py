#!/usr/bin/env python3
# -*- coding: utf-8 -*-
from .config import *
from .video_capture import *
from .box_detector import *
from message_bus import *


class FaceDetector(object):
    def __init__(self, video_capture):
        self.video_capture = video_capture
        sample_image = self.video_capture.capture()
        self.image_height, self.image_width = (
            sample_image.shape[0],
            sample_image.shape[1],
        )
        self.box_detector = BoxDetector()
        self.publisher = Publisher(ZMQ_FACE_DETECTOR)

    def detect(self):
        while True:
            img = self.video_capture.capture()
            self.publisher.send(b"image", img)
            boxes = self.box_detector.detect(img)
            if boxes:
                # NOTE: only one face is detected
                self.publisher.send(b"box", boxes[0])


if __name__ == "__main__":
    if VIDEO_CAPTURE == "webcam":
        detector = FaceDetector(WebCamVideoCapture())
    else:
        detector = FaceDetector(InuVideoCapture())

    detector.detect()
