#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# 2021-04-13 15:09
from config import *

if BACKBONE == "vgg":
    from .vgg import VGG as Backbone
    from .vgg import *
elif BACKBONE == "mobilenet":
    from .mobilenet import MobileNet as Backbone
    from .mobilenet import *
else:
    raise Exception(f"unknown backbone: {BACKBONE}")
