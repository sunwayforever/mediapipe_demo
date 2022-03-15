#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# 2021-03-07 19:45
import importlib
import threading
import os
import argparse

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--apps", type=str, default="", help="app1,app2,...")
    FLAGS = parser.parse_args()

    apps = []
    if FLAGS.apps:
        apps = FLAGS.apps.split(",")
    else:
        for root, _, files in os.walk("."):
            root = root[2:].replace("/", ".")
            for f in files:
                if f.endswith("main.py"):
                    apps.append(root)

    apps = [f"{app}.main" for app in apps]

    threads = []
    for app in apps:
        app = importlib.import_module(app)
        t = threading.Thread(None, app.run)  # type:ignore
        t.start()
        threads.append(t)

    for t in threads:
        t.join()
