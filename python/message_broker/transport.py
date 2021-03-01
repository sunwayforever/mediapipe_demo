#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# 2021-02-23 11:19
import zmq
import pickle
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
        if self.throttler.check(topic):
            self.sock.send_multipart([topic, pickle.dumps(data)])


class Subscriber(object):
    def __init__(self):
        self.ctx = zmq.Context()
        self.sock = self.ctx.socket(zmq.SUB)
        self.sock.connect(f"tcp://127.0.0.1:{OUTPUT_PORT}")

    def sub(self, topics, callback):
        for topic in topics:
            self.sock.subscribe(topic)
        self.callback = callback
        return self

    def _recv(self):
        raw_data = self.sock.recv_multipart()
        topic, data = raw_data[0], pickle.loads(raw_data[1])
        self.callback(topic, data)

    def loop(self):
        while True:
            self._recv()


class PolledSubscriber(object):
    def __init__(self):
        self.poller = zmq.Poller()
        self.ctx = zmq.Context()
        self.callbacks = {}

    def sub(self, topic, callback):
        sock = self.ctx.socket(zmq.SUB)
        sock.connect(f"tcp://127.0.0.1:{OUTPUT_PORT}")
        for topic in topics:
            sock.subscribe(topic)
        self.poller.register(sock, zmq.POLLIN)
        self.callbacks[sock] = callback
        return self

    def _recv(self):
        ready_socks = dict(poller.poll())
        for sock in ready_socks.keys():
            raw_data = sock.recv_multipart()
            topic, data = raw_data[0], pickle.loads(raw_data[1])
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
