#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# 2021-02-19 17:08

### https://stackoverflow.com/questions/39975676/how-to-implement-prelu-activation-in-tensorflow

import os
import numpy as np
import json
import tensorflow.compat.v1 as tf
import shutil
from pathlib import Path

schema = "../../model/schema.fbs"
binary = os.path.expanduser(
    "~/source/tensorflow/bazel-out/host/bin/external/flatbuffers/flatc"
)
model_path = "../../model/face_landmark.tflite"
output_savedmodel_path = "../../model/face_landmark"
model_json_path = "../../model/face_landmark.json"

num_tensors = 214
output_node_names = ["conv2d_20", "conv2d_30"]


def gen_model_json():
    if not os.path.exists(model_json_path):
        cmd = (
            binary
            + " -t --strict-json --defaults-json -o ../../model {schema} -- {input}".format(
                input=model_path, schema=schema
            )
        )
        print("output json command =", cmd)
        os.system(cmd)


def parse_json():
    j = json.load(open(model_json_path))
    op_types = [v["builtin_code"] for v in j["operator_codes"]]
    print("op types:", op_types)
    ops = j["subgraphs"][0]["operators"]
    print("num of ops:", len(ops))
    return ops, op_types


def make_graph(ops, op_types, interpreter):
    tensors = {}
    input_details = interpreter.get_input_details()
    output_details = interpreter.get_output_details()
    print(input_details)
    for input_detail in input_details:
        tensors[input_detail["index"]] = tf.compat.v1.placeholder(
            dtype=input_detail["dtype"],
            shape=input_detail["shape"],
            name=input_detail["name"],
        )

    for index, op in enumerate(ops):
        print("op: ", op)
        op_type = op_types[op["opcode_index"]]
        if op_type == "CONV_2D":
            input_tensor = tensors[op["inputs"][0]]
            weights_detail = interpreter._get_tensor_details(op["inputs"][1])
            bias_detail = interpreter._get_tensor_details(op["inputs"][2])
            output_detail = interpreter._get_tensor_details(op["outputs"][0])
            # print('weights_detail: ', weights_detail)
            # print('bias_detail: ', bias_detail)
            # print('output_detail: ', output_detail)
            weights_array = interpreter.get_tensor(weights_detail["index"])
            weights_array = np.transpose(weights_array, (1, 2, 3, 0))
            bias_array = interpreter.get_tensor(bias_detail["index"])
            weights = tf.Variable(weights_array, name=weights_detail["name"])
            bias = tf.Variable(bias_array, name=bias_detail["name"])
            options = op["builtin_options"]
            output_tensor = tf.nn.conv2d(
                input_tensor,
                weights,
                strides=[1, options["stride_h"], options["stride_w"], 1],
                padding=options["padding"],
                dilations=[
                    1,
                    options["dilation_h_factor"],
                    options["dilation_w_factor"],
                    1,
                ],
                name=output_detail["name"] + "/conv2d",
            )
            output_tensor = tf.add(output_tensor, bias, name=output_detail["name"])
            tensors[output_detail["index"]] = output_tensor
        elif op_type == "DEPTHWISE_CONV_2D":
            input_tensor = tensors[op["inputs"][0]]
            weights_detail = interpreter._get_tensor_details(op["inputs"][1])
            bias_detail = interpreter._get_tensor_details(op["inputs"][2])
            output_detail = interpreter._get_tensor_details(op["outputs"][0])
            # print('weights_detail: ', weights_detail)
            # print('bias_detail: ', bias_detail)
            # print('output_detail: ', output_detail)
            weights_array = interpreter.get_tensor(weights_detail["index"])
            weights_array = np.transpose(weights_array, (1, 2, 3, 0))
            bias_array = interpreter.get_tensor(bias_detail["index"])
            weights = tf.Variable(weights_array, name=weights_detail["name"])
            bias = tf.Variable(bias_array, name=bias_detail["name"])
            options = op["builtin_options"]
            output_tensor = tf.nn.depthwise_conv2d(
                input_tensor,
                weights,
                strides=[1, options["stride_h"], options["stride_w"], 1],
                padding=options["padding"],
                # dilations=[
                #     1, options['dilation_h_factor'],
                #     options['dilation_w_factor'], 1
                # ],
                name=output_detail["name"] + "/depthwise_conv2d",
            )
            output_tensor = tf.add(output_tensor, bias, name=output_detail["name"])
            tensors[output_detail["index"]] = output_tensor
        elif op_type == "MAX_POOL_2D":
            input_tensor = tensors[op["inputs"][0]]
            output_detail = interpreter._get_tensor_details(op["outputs"][0])
            options = op["builtin_options"]
            output_tensor = tf.nn.max_pool(
                input_tensor,
                ksize=[1, options["filter_height"], options["filter_width"], 1],
                strides=[1, options["stride_h"], options["stride_w"], 1],
                padding=options["padding"],
                name=output_detail["name"],
            )
            tensors[output_detail["index"]] = output_tensor
        elif op_type == "PAD":
            input_tensor = tensors[op["inputs"][0]]
            output_detail = interpreter._get_tensor_details(op["outputs"][0])
            paddings_detail = interpreter._get_tensor_details(op["inputs"][1])
            # print('input_tensor: ', input_tensor)
            # print('output_detail:', output_detail)
            # print('paddings_detail:', paddings_detail)
            paddings_array = interpreter.get_tensor(paddings_detail["index"])
            # paddings = tf.Variable(
            #    paddings_array, name=paddings_detail['name'])
            # output_tensor = tf.pad(
            #    input_tensor, paddings, name=output_detail['name'])
            output_tensor = tf.pad(
                input_tensor, paddings_array, name=output_detail["name"]
            )
            tensors[output_detail["index"]] = output_tensor
        elif op_type == "RELU":
            output_detail = interpreter._get_tensor_details(op["outputs"][0])
            input_tensor = tensors[op["inputs"][0]]
            output_tensor = tf.nn.relu(input_tensor, name=output_detail["name"])
            tensors[output_detail["index"]] = output_tensor

        elif op_type == "PRELU":
            output_detail = interpreter._get_tensor_details(op["outputs"][0])
            input_tensor = tensors[op["inputs"][0]]
            alpha_detail = interpreter._get_tensor_details(op["inputs"][1])
            alpha_array = interpreter.get_tensor(alpha_detail["index"])
            with tf.compat.v1.variable_scope(name_or_scope=output_detail["name"]):
                alphas = tf.Variable(alpha_array, name=alpha_detail["name"])
                # pos = tf.nn.relu(input_tensor)
                # neg = alphas * (input_tensor - abs(input_tensor)) * 0.5
                # output_tensor = pos + neg
                output_tensor = tf.maximum(alphas * input_tensor, input_tensor)
            # print("PRELU.output_tensor=", output_tensor)
            tensors[output_detail["index"]] = output_tensor

        elif op_type == "RESHAPE":
            input_tensor = tensors[op["inputs"][0]]
            output_detail = interpreter._get_tensor_details(op["outputs"][0])
            options = op["builtin_options"]
            output_tensor = tf.reshape(
                input_tensor, options["new_shape"], name=output_detail["name"]
            )
            tensors[output_detail["index"]] = output_tensor
        elif op_type == "ADD":
            output_detail = interpreter._get_tensor_details(op["outputs"][0])
            input_tensor_0 = tensors[op["inputs"][0]]
            input_tensor_1 = tensors[op["inputs"][1]]
            output_tensor = tf.add(
                input_tensor_0, input_tensor_1, name=output_detail["name"]
            )
            tensors[output_detail["index"]] = output_tensor
        elif op_type == "CONCATENATION":
            output_detail = interpreter._get_tensor_details(op["outputs"][0])
            input_tensor_0 = tensors[op["inputs"][0]]
            input_tensor_1 = tensors[op["inputs"][1]]
            options = op["builtin_options"]
            output_tensor = tf.concat(
                [input_tensor_0, input_tensor_1],
                options["axis"],
                name=output_detail["name"],
            )
            tensors[output_detail["index"]] = output_tensor
        else:
            raise ValueError(op_type)


def main():

    tf.compat.v1.disable_eager_execution()

    gen_model_json()
    ops, op_types = parse_json()

    interpreter = tf.lite.Interpreter(model_path)
    interpreter.allocate_tensors()
    input_details = interpreter.get_input_details()
    output_details = interpreter.get_output_details()
    print(input_details)
    print(output_details)
    for i in range(num_tensors):
        detail = interpreter._get_tensor_details(i)
        print(detail)

    make_graph(ops, op_types, interpreter)

    config = tf.compat.v1.ConfigProto()
    config.gpu_options.allow_growth = True
    graph = tf.compat.v1.get_default_graph()
    # writer = tf.summary.FileWriter(os.path.splitext(output_pb_path)[0])
    # writer.add_graph(graph)
    # writer.flush()
    # writer.close()
    with tf.compat.v1.Session(config=config, graph=graph) as sess:
        sess.run(tf.compat.v1.global_variables_initializer())
        graph_def = tf.compat.v1.graph_util.convert_variables_to_constants(
            sess=sess,
            input_graph_def=graph.as_graph_def(),
            output_node_names=output_node_names,
        )

        shutil.rmtree("saved_model", ignore_errors=True)
        tf.compat.v1.saved_model.simple_save(
            sess,
            output_savedmodel_path,
            inputs={"input": graph.get_tensor_by_name("input_1:0")},
            outputs={
                "conv2d_20": graph.get_tensor_by_name("conv2d_20:0"),
                "conv2d_30": graph.get_tensor_by_name("conv2d_30:0"),
            },
        )


if __name__ == "__main__":
    main()

"""
$ saved_model_cli show --dir saved_model --all

MetaGraphDef with tag-set: 'serve' contains the following SignatureDefs:

signature_def['serving_default']:
  The given SavedModel SignatureDef contains the following input(s):
    inputs['input'] tensor_info:
        dtype: DT_FLOAT
        shape: (1, 192, 192, 3)
        name: input_1:0
  The given SavedModel SignatureDef contains the following output(s):
    outputs['conv2d_20'] tensor_info:
        dtype: DT_FLOAT
        shape: (-1, 1, 1, 1404)
        name: conv2d_20:0
    outputs['conv2d_30'] tensor_info:
        dtype: DT_FLOAT
        shape: (-1, 1, 1, 1)
        name: conv2d_30:0
  Method name is: tensorflow/serving/predict
"""
