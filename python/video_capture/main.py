#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# 2021-03-02 10:35
from .video_capture import WebCamVideoCapture, InuVideoCapture
from message_broker.transport import Publisher

if __name__ == "__main__":
    vc = WebCamVideoCapture()
    publisher = Publisher()
    while True:
        # ZMQ_PUB: image
        publisher.pub(b"image", vc.capture())
