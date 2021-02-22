#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# 2021-02-20 14:46
import zmq
import pickle

ZMQ_FACE_DETECTOR = 5555


class Publisher(object):
    def __init__(self, port):
        self.ctx = zmq.Context()
        self.sock = self.ctx.socket(zmq.PUB)
        self.sock.bind(f"tcp://*:{port}")

    def send(self, topic, data):
        self.sock.send_multipart([topic, pickle.dumps(data)])


class Subscriber(object):
    def __init__(self, port, callback):
        self.ctx = zmq.Context()
        self.sock = self.ctx.socket(zmq.SUB)
        self.sock.connect(f"tcp://127.0.0.1:{port}")
        self.sock.subscribe(b"")
        self.callback = callback

    def recv(self):
        raw_data = self.sock.recv_multipart()
        topic, data = raw_data[0], pickle.loads(raw_data[1])
        self.callback(topic, data)
