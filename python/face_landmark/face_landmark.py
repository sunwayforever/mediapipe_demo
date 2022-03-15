#!/usr/bin/env python3
# -*- coding: utf-8 -*-
import math
import cv2
import numpy as np
import MNN

from .config import *
from message_broker import Publisher

from common import util


class FaceLandmark(object):
    def __init__(self):
        self.interpreter = MNN.Interpreter(util.get_resource(MODEL))
        self.session = self.interpreter.createSession()
        self.input_tensor = self.interpreter.getSessionInput(self.session)
        self.publisher = Publisher()
        self.mean = np.array([0.485 * 255, 0.456 * 255, 0.406 * 255], dtype="float32")
        self.norm = np.array(
            [1.0 / (0.229 * 255), 1.0 / (0.224 * 255), 1.0 / (0.225 * 255)],
            dtype="float32",
        )
        self.image = None
        self.face_box = None

    def __call__(self, topic, data):
        if topic == b"image":
            self.image = data
        if topic == b"face_box":
            self.face_box = data
        if self.image is None or self.face_box is None:
            return
        self.detect()

    @staticmethod
    def crop(image, box):
        x1 = box.xmin
        y1 = box.ymin
        x2 = box.xmin + box.width
        y2 = box.ymin + box.height

        # NOTE: squred crop
        x1, y1, x2, y2 = util.square_rect(x1, y1, x2, y2)

        # NOTE: scale up to 1.25x
        margin_w, margin_h = (x2 - x1) // 4, (y2 - y1) // 4

        image = image[
            max(y1 - margin_h, 0) : y2 + margin_h, max(x1 - margin_w, 0) : x2 + margin_w
        ]

        left_eye, right_eye = box.keypoints[0], box.keypoints[1]
        angle = (
            math.atan2(right_eye[1] - left_eye[1], right_eye[0] - left_eye[0]) * 57.3
        )
        rot_mat = cv2.getRotationMatrix2D(
            (image.shape[0] / 2, image.shape[1] / 2), angle, 1
        )
        image = cv2.warpAffine(image, rot_mat, (image.shape[1], image.shape[0]))

        translation_mat = util.get_translation_mat(x1 - margin_w, y1 - margin_h)
        rotation_mat = cv2.getRotationMatrix2D(
            (image.shape[0] / 2, image.shape[1] / 2), -angle, 1
        )
        # convert to homogeneous coordinates
        rotation_mat = np.vstack([rotation_mat, [0, 0, 1]])
        return util.ROIImage(image, translation_mat @ rotation_mat)

    def detect(self):
        # NOTE: crop and rotate
        roi_image, mat = FaceLandmark.crop(self.image, self.face_box)
        # NOTE: resize
        scale_mat = util.get_scale_mat(
            roi_image.shape[1] / IMG_WIDTH,
            roi_image.shape[0] / IMG_HEIGHT,
        )
        mat = mat @ scale_mat
        roi_image = cv2.resize(roi_image, (int(IMG_WIDTH), int(IMG_HEIGHT)))
        # NOTE: convert to rgb
        roi_image = cv2.cvtColor(roi_image, cv2.COLOR_BGR2RGB)
        # NOTE: normalize
        roi_image = roi_image.astype("float32")
        roi_image -= self.mean
        roi_image *= self.norm

        roi_image = roi_image.transpose((2, 0, 1))
        tmp_image = MNN.Tensor(
            (1, 3, IMG_HEIGHT, IMG_WIDTH),
            MNN.Halide_Type_Float,
            roi_image,
            MNN.Tensor_DimensionType_Caffe,
        )
        self.input_tensor.copyFrom(tmp_image)
        self.interpreter.runSession(self.session)
        angle, _, landmark, visible = [
            x.getData().squeeze()
            for x in self.interpreter.getSessionOutputAll(self.session).values()
        ]
        rect = np.array([IMG_WIDTH, IMG_HEIGHT])
        landmark = util.restore_coords_2d(landmark.reshape((-1, 2)) * rect, mat).astype(
            "int"
        )
        self.publisher.pub(b"face_landmark", landmark)
