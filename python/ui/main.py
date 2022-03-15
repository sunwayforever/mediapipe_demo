#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# 2022-03-14 17:05
#!/usr/bin/env python3
# -*- coding: utf-8 -*-
from message_broker import Subscriber
from .cv_display import CVDisplay

def run():
    Subscriber().sub(
        [
            b"image",
            b"face_box",
            b"face_landmark", 
        ],
        CVDisplay(),
    ).loop()


if __name__ == "__main__":
    run()
