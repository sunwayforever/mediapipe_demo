#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# 2021-02-20 11:29
def detect_image(img):
    detector = Detector()
    img = cv2.imread(img)
    pose_estimator = PoseEstimator((img.shape[0], img.shape[1]))

    do_detect(detector, pose_estimator, img)

    while cv2.waitKey(1) & 0xFF != ord("q"):
        pass
    cv2.destroyAllWindows()
