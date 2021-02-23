#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# 2021-02-20 14:46

from .transport import *
from .config import *

# the broker will sub on INPUT_PORT on pub to OUTPUT_PORT
if __name__ == "__main__":
    ctx = zmq.Context()
    sub_sock = ctx.socket(zmq.SUB)
    sub_sock.bind(f"tcp://127.0.0.1:{INPUT_PORT}")
    sub_sock.subscribe(b"")

    pub_sock = ctx.socket(zmq.PUB)
    pub_sock.bind(f"tcp://127.0.0.1:{OUTPUT_PORT}")

    while True:
        pub_sock.send_multipart(sub_sock.recv_multipart())
