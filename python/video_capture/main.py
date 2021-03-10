#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# 2021-03-02 10:35
from .video_capture import WebCamVideoCapture, InuVideoCapture
from message_broker.transport import Publisher
from .config import *


def run():
    if DEVICE == "webcam":
        vc = WebCamVideoCapture()
    else:
        vc = InuVideoCapture()
    publisher = Publisher()
    while True:
        # ZMQ_PUB: image
        image = vc.capture()
        if image is not None:
            publisher.pub(b"image", image)


if __name__ == "__main__":
    run()
