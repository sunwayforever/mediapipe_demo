#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# 2021-02-22 16:17
from PyQt5.QtGui import QGuiApplication, QImage
from PyQt5.QtQml import QQmlApplicationEngine
from PyQt5.QtCore import QRunnable, QThreadPool, Qt, pyqtSignal, QObject

from message_broker import *


class MessageHandler(QRunnable):
    def __init__(self, topics, callback):
        QRunnable.__init__(self)
        self.callback = callback
        self.subscriber = Subscriber(topics, callback)

    def run(self):
        while True:
            self.subscriber.recv()


class Router(object):
    def __init__(self):
        self.handlers = []

    def add_route(self, topics, callback):
        self.handlers.append(MessageHandler(topics, callback))

    def start(self):
        for handler in self.handlers:
            QThreadPool.globalInstance().start(handler)
