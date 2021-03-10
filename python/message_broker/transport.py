#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# 2021-02-23 11:19
import zmq
import pickle
from PyQt5.QtCore import QRunnable, QThreadPool

from .config import *
from .throttler import Throttler


class Publisher(object):
    def __init__(self):
        self.throttler = Throttler()
        self.ctx = zmq.Context()
        self.sock = self.ctx.socket(zmq.PUB)
        self.sock.connect(INBOUND_ADDR)

    def pub(self, topic, data=None):
        if self.throttler.is_send_allowed(topic):
            self.sock.send(topic + b":" + pickle.dumps(data))


class Subscriber(object):
    def __init__(self):
        self.poller = zmq.Poller()
        self.ctx = zmq.Context()
        self.callback = None

    def sub(self, topics, callback):
        if isinstance(topics, bytes):
            topics = [topics]
        for topic in topics:
            sock = self.ctx.socket(zmq.SUB)
            sock.setsockopt(zmq.CONFLATE, 1)
            sock.connect(OUTBOUND_ADDR)
            sock.subscribe(topic)
            self.poller.register(sock, zmq.POLLIN)
            self.callback = callback
        return self

    def _recv(self):
        ready_socks = dict(self.poller.poll())
        for sock in ready_socks.keys():
            raw_data = sock.recv()
            index = raw_data.index(b":")
            topic, data = (
                raw_data[:index],
                pickle.loads(raw_data[index + 1 :]),
            )
            self.callback(topic, data)

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
