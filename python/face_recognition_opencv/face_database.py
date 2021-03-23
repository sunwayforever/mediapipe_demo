#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# 2021-03-01 12:07
import os
import pickle
import cv2

from .config import *
from common import util


class FaceDatabase(object):
    def __init__(self):
        # self.orb = cv2.ORB_create(nfeatures=256)
        self.sift = cv2.SIFT_create()
        self.matcher = cv2.BFMatcher()
        self.image_db_file = util.get_resource("../data/image.db")
        if os.path.exists(self.image_db_file):
            with open(self.image_db_file, "rb") as f:
                self.image_db = pickle.load(f)
        else:
            self.image_db = []

    def query(self, target_image):
        if len(self.image_db) == 0:
            return None
        gray_image = cv2.cvtColor(target_image, cv2.COLOR_BGR2GRAY)
        # _, target_des = self.orb.detectAndCompute(gray_image, None)
        _, target_des = self.sift.detectAndCompute(gray_image, None)
        if target_des is None:
            return None
        for candidate_des, candidate_image in self.image_db:
            matches = self.matcher.knnMatch(target_des, candidate_des, k=2)
            # https://stackoverflow.com/questions/51197091/how-does-the-lowes-ratio-test-work
            good_matches = [
                m[0]
                for m in matches
                if len(m) == 2 and m[0].distance < m[1].distance * 0.75
            ]
            print(f"query: {len(good_matches)}")
            if len(good_matches) > 10:
                return candidate_image
        return None

    def enroll(self, image):
        gray_image = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
        # _, des = self.orb.detectAndCompute(gray_image, None)
        _, des = self.sift.detectAndCompute(gray_image, None)
        print(f"enroll: {des.shape}")
        self.image_db.append((des, image))
        with open(self.image_db_file, "wb") as f:
            pickle.dump(self.image_db, f)
