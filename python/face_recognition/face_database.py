#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# 2021-03-01 12:07
from annoy import AnnoyIndex
import pickledb
import os

from .config import *
import util


class FaceDatabase(object):
    def __init__(self):
        self.index = AnnoyIndex(EMBEDDING_SIZE, "angular")
        facenet_ann = util.get_resource("../data/facenet.ann")

        if os.path.exists(facenet_ann):
            self.index.load(facenet_ann())
        else:
            self.index.build(10)
        self.image_db = pickledb.load(
            util.get_resource("../data/image.db"), auto_dump=False
        )

    def query(self, embedding):
        index, distance = self.index.get_nns_by_vector(
            embedding, 1, include_distances=True
        )
        if len(index) == 0:
            return None

        index, distance = index[0], distance[0]
        face = self.image_db.get(str(index))

        if distance > FACE_DISTANCE_THRESHOLD:
            return None
        if face is None:
            print("error: missing face")
            return None
        return face
