#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# 2021-02-23 11:19
import zmq
import pickle
from .config import *


class Publisher(object):
    def __init__(self):
        self.ctx = zmq.Context()
        self.sock = self.ctx.socket(zmq.PUB)
        self.sock.connect(f"tcp://127.0.0.1:{INPUT_PORT}")

    def pub(self, topic, data):
        self.sock.send_multipart([topic, pickle.dumps(data)])


class Subscriber(object):
    def __init__(self, topics, callback):
        self.ctx = zmq.Context()
        self.sock = self.ctx.socket(zmq.SUB)
        self.sock.connect(f"tcp://127.0.0.1:{OUTPUT_PORT}")
        for topic in topics:
            self.sock.subscribe(topic)
        self.callback = callback

    def _recv(self):
        raw_data = self.sock.recv_multipart()
        topic, data = raw_data[0], pickle.loads(raw_data[1])
        self.callback(topic, data)

    def loop(self):
        while True:
            self._recv()
