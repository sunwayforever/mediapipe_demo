#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# 2021-02-19 12:51

import os
import sys
import numpy as np
import json
import tensorflow as tf

schema = "../model/schema.fbs"
binary = os.path.expanduser(
    "~/source/tensorflow/bazel-out/host/bin/external/flatbuffers/flatc"
)

model_path = sys.argv[1]
model_name = os.path.basename(model_path).replace(".tflite", "")
model_json_path = f"/tmp/{model_name}.json"


def gen_model_json():
    if not os.path.exists(model_json_path):
        cmd = (
            binary
            + " -t --strict-json --defaults-json -o /tmp {schema} -- {input}".format(
                input=model_path, schema=schema
            )
        )
        print("output json command =", cmd)
        os.system(cmd)


def parse_json():
    j = json.load(open(model_json_path))
    op_types = [v["builtin_code"] for v in j["operator_codes"]]
    ops = j["subgraphs"][0]["operators"]
    return ops, op_types


def make_graph(ops, op_types, interpreter):
    flops = 0
    ignored_op = {}
    for index, op in enumerate(ops):
        op_type = op_types[op["opcode_index"]]
        if op_type == "CONV_2D":
            # - input 为 [1, x1, y1, c]
            # - filter 为 [o, a, b, c]
            # - output 为 [1, x2, y2, o]
            #
            # FLOPs 为 a*b*c*2*(x2*y2*o)

            # input shape: [ 1 16 16 88]
            # kernel shape: [32  1  1 88]
            # output shape: [ 1 16 16 32]

            input_shape = interpreter._get_tensor_details(op["inputs"][0])["shape"]
            kernel_shape = interpreter._get_tensor_details(op["inputs"][1])["shape"]
            output_shape = interpreter._get_tensor_details(op["outputs"][0])["shape"]
            flops += (
                kernel_shape[0]
                * kernel_shape[1]
                * kernel_shape[2]
                * kernel_shape[3]
                * output_shape[1]
                * output_shape[2]
                * 2
            )
        elif op_type == "DEPTHWISE_CONV_2D":
            input_shape = interpreter._get_tensor_details(op["inputs"][0])["shape"]
            kernel_shape = interpreter._get_tensor_details(op["inputs"][1])["shape"]
            output_shape = interpreter._get_tensor_details(op["outputs"][0])["shape"]
            flops += (
                kernel_shape[0]
                * kernel_shape[1]
                * kernel_shape[2]
                * output_shape[1]
                * output_shape[2]
                * 2
            )
        elif op_type == "ADD":
            input_shape = interpreter._get_tensor_details(op["inputs"][0])["shape"]
            flops += np.prod(input_shape)
        elif op_type == "FULLY_CONNECTED":
            weight_shape = interpreter._get_tensor_details(op["inputs"][1])["shape"]
            flops += np.prod(input_shape) * 2
        else:
            if op_type not in ignored_op:
                print("ignored ", op_type)
                ignored_op[op_type] = 1
    print(
        f"total flops: {flops / 1024 / 1024} M",
    )


def main():
    tf.compat.v1.disable_eager_execution()
    gen_model_json()
    ops, op_types = parse_json()
    interpreter = tf.lite.Interpreter(model_path)
    interpreter.allocate_tensors()
    make_graph(ops, op_types, interpreter)


if __name__ == "__main__":
    main()
