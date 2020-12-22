#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# 2020-12-22 14:58
import cv2
import argparse
import numpy as np


def resize(img, roi_width, roi_height):
    # height/width
    orig_aspect_ratio = img.shape[0] / img.shape[1]
    roi_aspect_ratio = roi_width / roi_height
    h_padding, v_padding = 0, 0

    if orig_aspect_ratio < roi_aspect_ratio:
        new_width = roi_width
        new_height = int(roi_width * orig_aspect_ratio)
        v_padding = roi_height - new_height
    else:
        new_height = roi_height
        new_width = int(roi_height / orig_aspect_ratio)
        h_padding = roi_width - new_width

    img = cv2.resize(img, (int(new_width), int(new_height)))
    v_padding = int(v_padding / 2)
    h_padding = int(h_padding / 2)
    img = cv2.copyMakeBorder(
        img,
        v_padding,
        v_padding,
        h_padding,
        h_padding,
        cv2.BORDER_CONSTANT,
        value=(0, 0, 0),
    )
    return img, v_padding / roi_width, h_padding / roi_height


if __name__ == "__main__":
    vid = cv2.VideoCapture(0)
    while True:
        _, orig_img = vid.read()
        img, _, _ = resize(orig_img, 128, 128)
        img2 = cv2.resize(orig_img, (128, 128))
        cv2.imshow("", np.concatenate([img, img2]))
        if cv2.waitKey(1) & 0xFF == ord("q"):
            break

    vid.release()
    cv2.destroyAllWindows()

    # parser = argparse.ArgumentParser()
    # parser.add_argument("file_name", type=str)
    # flags = parser.parse_args()
    # orig_img = cv2.imread(flags.file_name)
    # img, _, _ = resize(orig_img, 128, 128)
    # img2 = cv2.resize(orig_img, (128, 128))
    # cv2.imshow("", np.concatenate([img, img2]))
    # while cv2.waitKey(1) & 0xFF != ord("q"):
    #     pass
