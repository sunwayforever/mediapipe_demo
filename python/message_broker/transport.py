#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# 2021-02-23 11:19
import zmq
import pickle
import time
import struct
from PyQt5.QtCore import QRunnable, QThreadPool

from .throttler import Throttler
from .config import *


class Publisher(object):
    def __init__(self):
        self.throttler = Throttler()
        self.ctx = zmq.Context()
        self.sock = self.ctx.socket(zmq.PUB)
        self.sock.connect(f"tcp://127.0.0.1:{INPUT_PORT}")

    def pub(self, topic, data=None):
        if self.throttler.is_send_allowed(topic):
            if DEBUG:
                print("pub: ", topic)
            self.sock.send_multipart(
                [topic, struct.pack("<Q", int(time.time() * 1000)), pickle.dumps(data)]
            )


class Subscriber(object):
    def __init__(self):
        self.throttler = Throttler()
        self.ctx = zmq.Context()
        self.sock = self.ctx.socket(zmq.SUB)
        self.sock.connect(f"tcp://127.0.0.1:{OUTPUT_PORT}")
        self.callback = None

    def sub(self, topics, callback):
        if self.callback is not None:
            raise Exception("mulptiple `sub` invoked")
        if isinstance(topics, bytes):
            topics = [topics]
        for topic in topics:
            self.sock.subscribe(topic)
        self.callback = callback
        return self

    def _recv(self):
        raw_data = self.sock.recv_multipart()
        topic, time, data = (
            raw_data[0],
            struct.unpack("<Q", raw_data[1])[0],
            pickle.loads(raw_data[2]),
        )
        if self.throttler.is_recv_allowed(topic, time):
            self.callback(topic, data)

    def loop(self):
        while True:
            self._recv()


class PolledSubscriber(object):
    def __init__(self):
        self.throttler = Throttler()
        self.poller = zmq.Poller()
        self.ctx = zmq.Context()
        self.callbacks = {}

    def sub(self, topics, callback):
        sock = self.ctx.socket(zmq.SUB)
        sock.connect(f"tcp://127.0.0.1:{OUTPUT_PORT}")
        for topic in topics:
            sock.subscribe(topic)
        self.poller.register(sock, zmq.POLLIN)
        self.callbacks[sock] = callback
        return self

    def _recv(self):
        ready_socks = dict(self.poller.poll())
        for sock in ready_socks.keys():
            raw_data = sock.recv_multipart()
            topic, time, data = raw_data[0], raw_data[1], pickle.loads(raw_data[2])
            if self.throttler.is_recv_allowed(topic, time):
                self.callbacks[sock](topic, data)

    def loop(self):
        while True:
            self._recv()


class _QThreadedSubscriberHandler(QRunnable):
    def __init__(self, topics, callback):
        QRunnable.__init__(self)
        self.callback = callback
        self.subscriber = Subscriber().sub(topics, callback)

    def run(self):
        self.subscriber.loop()


class QThreadedSubscriber(object):
    def __init__(self):
        self.handlers = []

    def sub(self, topics, callback):
        self.handlers.append(_QThreadedSubscriberHandler(topics, callback))
        return self

    def loop(self):
        for handler in self.handlers:
            QThreadPool.globalInstance().start(handler)
