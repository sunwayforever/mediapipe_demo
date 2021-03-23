#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# 2021-02-26 18:16
import cv2

# from .inception_resnet_v1 import InceptionResNetV1
from .config import *
from .face_database import FaceDatabase
from message_broker import Publisher


class FaceRecognizer:
    def __init__(self):
        self.face_db = FaceDatabase()
        self.publisher = Publisher()
        self.orb = cv2.ORB_create(nfeatures=256)
        self.last_face = None
        self.last_face_embedding = None

    def __call__(self, topic, data):
        if topic == b"face_roi_small":
            self.detect(data)
        if topic == b"enroll":
            self.enroll()

    def enroll(self):
        if self.last_face is not None:
            self.face_db.enroll(self.last_face)

    def detect(self, face):
        self.last_face = face
        # self.last_face_image = cv2.cvtColor(self.last_face_image, cv2.COLOR_BGR2GRAY)
        # ZMQ_PUB: facenet
        self.publisher.pub(b"facenet", self.face_db.query(self.last_face))
