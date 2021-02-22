#!/usr/bin/env python3
# -*- coding: utf-8 -*-
from .config import *
from .video_capture import WebCamVideoCapture, InuVideoCapture
from .box_detector import BoxDetector
from .face_cropper import FaceCropper
from message_broker import Publisher


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
            self.publisher.send(b"image", img)
            boxes = self.box_detector.detect(img)
            if not boxes:
                continue
            # NOTE: only one face is detected
            box = boxes[0]
            self.publisher.send(b"box", box)

            face = self.face_cropper.crop(img, box)
            self.publisher.send(b"face", face)


if __name__ == "__main__":
    if VIDEO_CAPTURE == "webcam":
        detector = FaceDetector(WebCamVideoCapture())
    else:
        detector = FaceDetector(InuVideoCapture())

    detector.detect()
