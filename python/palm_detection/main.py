#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# 2021-02-23 10:07
from .config import *
from .palm_detector import PalmDetector
from message_broker.transport import Subscriber

if __name__ == "__main__":
    Subscriber().sub([b"image"], PalmDetector()).loop()
