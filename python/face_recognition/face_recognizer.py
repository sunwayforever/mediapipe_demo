#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# 2021-02-26 18:16
from cv2 import cv2
import numpy as np

from .inception_resnet_v1 import InceptionResNetV1
from .config import *
from .face_database import FaceDatabase
from message_broker.transport import Publisher
from common import util


class FaceRecognizer(object):
    def __init__(self):
        self.model = InceptionResNetV1(
            weights_path=util.get_resource("../model/facenet-weights.h5")
        )
        self.face_db = FaceDatabase()
        self.publisher = Publisher()

        self.last_face_image = None
        self.last_face_embedding = None

    def __call__(self, topic, data):
        if topic == b"face_roi_slow":
            self.detect(data)
        if topic == b"enroll":
            self.enroll()

    def enroll(self):
        if self.last_face_image is not None:
            self.face_db.enroll(self.last_face_image, self.last_face_embedding)

    def detect(self, face_roi):
        self.last_face_image = face_roi.image
        face_image = cv2.resize(face_roi.image, (IMG_WIDTH, IMG_HEIGHT))
        face_image = np.expand_dims(face_image, axis=0)
        embedding = self.model(face_image).numpy().ravel()  # type: ignore
        self.last_face_embedding = embedding

        # ZMQ_PUB: facenet
        self.publisher.pub(b"facenet", self.face_db.query(embedding))
