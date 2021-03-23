#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# 2021-02-23 10:07
from .config import *
from .object_detector import ObjectDetector
from message_broker import Subscriber


def run():
    Subscriber().sub([b"image"], ObjectDetector()).loop()


if __name__ == "__main__":
    run()
