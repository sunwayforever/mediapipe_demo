#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# 2021-03-01 12:07
import os
import pickle

from .config import *
import util


class FaceDatabase(object):
    def __init__(self):
        self.image_db_file = util.get_resource("../data/image.db")
        if os.path.exists(self.image_db_file):
            with open(self.image_db_file, "rb") as f:
                self.image_db = pickle.load(f)
        else:
            self.image_db = []

    def query(self, embedding):
        if len(self.image_db) == 0:
            return None

        target = min(
            [[util.compute_distance(embedding, k), v] for (k, v) in self.image_db],
            key=lambda x: x[0],
        )
        print("distance:", target[0])
        if target[0] > FACE_DISTANCE_THRESHOLD:
            return None

        return target[1]

    def enroll(self, image, embedding):
        self.image_db.append((embedding, image))
        with open(self.image_db_file, "wb") as f:
            pickle.dump(self.image_db, f)
