#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# 2021-02-20 14:46
import zmq
import pickle

# the broker will sub on INPUT_PORT on pub to OUTPUT_PORT
#
INPUT_PORT = 5555
OUTPUT_PORT = 5556


class Publisher(object):
    def __init__(self):
        self.ctx = zmq.Context()
        self.sock = self.ctx.socket(zmq.PUB)
        self.sock.connect(f"tcp://127.0.0.1:{INPUT_PORT}")

    def send(self, topic, data):
        self.sock.send_multipart([topic, pickle.dumps(data)])


class Subscriber(object):
    def __init__(self, topics, callback):
        self.ctx = zmq.Context()
        self.sock = self.ctx.socket(zmq.SUB)
        self.sock.connect(f"tcp://127.0.0.1:{OUTPUT_PORT}")
        for topic in topics:
            self.sock.subscribe(topic)
        self.callback = callback

    def recv(self):
        raw_data = self.sock.recv_multipart()
        topic, data = raw_data[0], pickle.loads(raw_data[1])
        self.callback(topic, data)


if __name__ == "__main__":
    ctx = zmq.Context()
    sub_sock = ctx.socket(zmq.SUB)
    sub_sock.bind(f"tcp://127.0.0.1:{INPUT_PORT}")
    sub_sock.subscribe(b"")

    pub_sock = ctx.socket(zmq.PUB)
    pub_sock.bind(f"tcp://127.0.0.1:{OUTPUT_PORT}")

    while True:
        data = sub_sock.recv_multipart()
        pub_sock.send_multipart(data)
