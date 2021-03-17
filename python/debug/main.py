#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# 2021-02-24 17:36
from message_broker.transport import Subscriber

class Debugger(object):
    def __init__(self):
        pass

    def __call__(self, topic, data):
        if topic == b"image":


def run():
    # ZMQ_SUB:
    Subscriber().sub([b"image"], Debugger()).loop()


if __name__ == "__main__":
    run()
