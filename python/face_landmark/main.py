#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# 2021-02-23 10:07
from .config import *
from .face_landmark import FaceLandmark
from message_broker import Subscriber


def run():
    Subscriber().sub([b"image", b"face_box"], FaceLandmark()).loop()


if __name__ == "__main__":
    run()
