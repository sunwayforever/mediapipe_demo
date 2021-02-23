#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# 2021-02-23 10:07
from .config import *
from .face_detector import *
from .video_capture import WebCamVideoCapture, InuVideoCapture

if __name__ == "__main__":
    if VIDEO_CAPTURE == "webcam":
        detector = FaceDetector(WebCamVideoCapture())
    else:
        detector = FaceDetector(InuVideoCapture())

    detector.detect()
