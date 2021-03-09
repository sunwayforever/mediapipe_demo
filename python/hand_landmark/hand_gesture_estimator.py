#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# 2021-03-03 18:18


class HandGestureEstimator(object):
    # NEXT: left hand or right hand
    # NEXT: align hand before estimate gesture
    def __init__(self):
        self.mapping = {
            "|||||": "five",
            ".||||": "four",
            ".|||.": "three",
            ".||..": "two",
            ".|...": "one",
            "|....": "thumb",
        }

    def estimate(self, mark):
        finger_state = ["x"] * 5

        if mark[3][0] > mark[4][0]:
            finger_state[0] = "|"
        elif mark[3][0] < mark[4][0]:
            finger_state[0] = "."

        for i, j in zip(range(1, 5), [6, 10, 14, 18]):
            if mark[j][1] > mark[j + 1][1] and mark[j + 1][1] > mark[j + 2][1]:
                finger_state[i] = "|"
            elif mark[j][1] < mark[j + 1][1] and mark[j + 1][1] < mark[j + 2][1]:
                finger_state[i] = "."

        finger_state = str.join("", finger_state)
        return self.mapping.get(finger_state, "unknown")
