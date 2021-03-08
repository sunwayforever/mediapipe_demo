#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# 2021-03-05 22:01
from onnxruntime.quantization import quantize_dynamic
import argparse

parser = argparse.ArgumentParser()
parser.add_argument("onnx_float", type=str)
parser.add_argument("onnx_quant", type=str)
FLAGS = parser.parse_args()

quantized_model = quantize_dynamic(FLAGS.onnx_float, FLAGS.onnx_quant)
