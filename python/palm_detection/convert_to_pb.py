#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# 2021-03-02 12:54
import os
import numpy as np
import json
import tensorflow.compat.v1 as tf
import tensorflow as tfv2
import shutil
from pathlib import Path
import pprint

schema = "../../model/schema.fbs"
binary = os.path.expanduser(
    "~/source/tensorflow/bazel-out/host/bin/external/flatbuffers/flatc"
)
model_path = "../../model/palm_detection.tflite"
output_savedmodel_path = "../../model/palm_detection"
model_json_path = "../../model/palm_detection.json"
output_node_names = ["classificators", "regressors"]

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


def optimizing_hardswish_for_edgetpu(input_op, name=None):
    ret_op = input_op * tf.nn.relu6(input_op + 3) * 0.16666666
    return ret_op


def make_graph(ops, op_types, interpreter):

    height = 128
    width = 128

    tensors = {}
    input_details = interpreter.get_input_details()
    # output_details = interpreter.get_output_details()
    print(input_details)
    for input_detail in input_details:
        tensors[input_detail["index"]] = tf.placeholder(
            dtype=input_detail["dtype"],
            shape=input_detail["shape"],
            name=input_detail["name"],
        )

    for op in ops:
        print("@@@@@@@@@@@@@@ op:", op)
        op_type = op_types[op["opcode_index"]]
        if op_type == "CONV_2D":
            input_tensor = tensors[op["inputs"][0]]

            weights_detail = interpreter._get_tensor_details(op["inputs"][1])
            weights = interpreter.get_tensor(weights_detail["index"]).transpose(
                1, 2, 3, 0
            )

            bias_detail = interpreter._get_tensor_details(op["inputs"][2])
            bias = interpreter.get_tensor(bias_detail["index"])

            output_detail = interpreter._get_tensor_details(op["outputs"][0])
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

            if output_detail["name"].split("/")[-1] == "Relu6":
                output_tensor = tf.nn.relu6(output_tensor)

            tensors[output_detail["index"]] = output_tensor

        elif op_type == "DEPTHWISE_CONV_2D":
            output_detail = interpreter._get_tensor_details(op["outputs"][0])
            input_tensor = tensors[op["inputs"][0]]

            weights_detail = interpreter._get_tensor_details(op["inputs"][1])
            weights = interpreter.get_tensor(weights_detail["index"]).transpose(
                1, 2, 3, 0
            )

            bias_detail = interpreter._get_tensor_details(op["inputs"][2])
            bias = interpreter.get_tensor(bias_detail["index"])

            options = op["builtin_options"]
            output_tensor = tf.nn.depthwise_conv2d(
                input_tensor,
                weights,
                strides=[1, options["stride_h"], options["stride_w"], 1],
                padding=options["padding"],
                # dilations=[1, options['dilation_h_factor'], options['dilation_w_factor'], 1],
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
            paddings_array = interpreter.get_tensor(paddings_detail["index"])
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
            output_tensor = tf.keras.layers.PReLU(
                alpha_initializer=tf.keras.initializers.Constant(alpha_array),
                shared_axes=[1, 2],
            )(input_tensor)
            tensors[output_detail["index"]] = output_tensor

        elif op_type == "RELU6":
            output_detail = interpreter._get_tensor_details(op["outputs"][0])
            input_tensor = tensors[op["inputs"][0]]
            output_tensor = tf.nn.relu6(input_tensor, name=output_detail["name"])
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
            try:
                input_tensor_1 = tensors[op["inputs"][1]]
            except:
                param = interpreter._get_tensor_details(op["inputs"][1])
                input_tensor_1 = interpreter.get_tensor(param["index"])
            output_tensor = tf.add(
                input_tensor_0, input_tensor_1, name=output_detail["name"]
            )

            if output_detail["name"].split("/")[-1] == "Relu6":
                output_tensor = tf.nn.relu6(output_tensor)

            tensors[output_detail["index"]] = output_tensor

        elif op_type == "CONCATENATION":
            output_detail = interpreter._get_tensor_details(op["outputs"][0])
            input_tensor_0 = tensors[op["inputs"][0]]
            input_tensor_1 = tensors[op["inputs"][1]]
            try:
                input_tensor_2 = tensors[op["inputs"][2]]
                options = op["builtin_options"]
                output_tensor = tf.concat(
                    [input_tensor_0, input_tensor_1, input_tensor_2],
                    options["axis"],
                    name=output_detail["name"],
                )
            except:
                options = op["builtin_options"]
                output_tensor = tf.concat(
                    [input_tensor_0, input_tensor_1],
                    options["axis"],
                    name=output_detail["name"],
                )
            tensors[output_detail["index"]] = output_tensor

        elif op_type == "LOGISTIC":
            output_detail = interpreter._get_tensor_details(op["outputs"][0])
            input_tensor = tensors[op["inputs"][0]]
            output_tensor = tf.math.sigmoid(input_tensor, name=output_detail["name"])
            tensors[output_detail["index"]] = output_tensor

        elif op_type == "TRANSPOSE_CONV":
            input_tensor = tensors[op["inputs"][2]]
            weights_detail = interpreter._get_tensor_details(op["inputs"][1])
            output_shape_detail = interpreter._get_tensor_details(op["inputs"][0])
            output_detail = interpreter._get_tensor_details(op["outputs"][0])
            weights_array = interpreter.get_tensor(weights_detail["index"])
            weights_array = np.transpose(weights_array, (1, 2, 0, 3))
            output_shape_array = interpreter.get_tensor(output_shape_detail["index"])
            weights = tf.Variable(weights_array, name=weights_detail["name"])
            shape = tf.Variable(output_shape_array, name=output_shape_detail["name"])
            options = op["builtin_options"]
            output_tensor = tf.nn.conv2d_transpose(
                input_tensor,
                weights,
                shape,
                [1, options["stride_h"], options["stride_w"], 1],
                padding=options["padding"],
                name=output_detail["name"] + "/conv2d_transpose",
            )
            tensors[output_detail["index"]] = output_tensor

        elif op_type == "MUL":
            output_detail = interpreter._get_tensor_details(op["outputs"][0])
            input_tensor_0 = tensors[op["inputs"][0]]
            input_tensor_1 = None
            try:
                input_tensor_1 = tensors[op["inputs"][1]]
            except:
                param = interpreter._get_tensor_details(op["inputs"][1])
                input_tensor_1 = interpreter.get_tensor(param["index"])
            output_tensor = tf.multiply(
                input_tensor_0, input_tensor_1, name=output_detail["name"]
            )
            tensors[output_detail["index"]] = output_tensor

        elif op_type == "HARD_SWISH":
            output_detail = interpreter._get_tensor_details(op["outputs"][0])
            input_tensor = tensors[op["inputs"][0]]
            output_tensor = optimizing_hardswish_for_edgetpu(
                input_tensor, name=output_detail["name"]
            )
            tensors[output_detail["index"]] = output_tensor

        elif op_type == "AVERAGE_POOL_2D":
            output_detail = interpreter._get_tensor_details(op["outputs"][0])
            input_tensor = tensors[op["inputs"][0]]
            options = op["builtin_options"]
            pool_size = [options["filter_height"], options["filter_width"]]
            strides = [options["stride_h"], options["stride_w"]]
            padding = options["padding"]
            output_tensor = tf.keras.layers.AveragePooling2D(
                pool_size=pool_size,
                strides=strides,
                padding=padding,
                name=output_detail["name"],
            )(input_tensor)
            tensors[output_detail["index"]] = output_tensor

        elif op_type == "FULLY_CONNECTED":
            output_detail = interpreter._get_tensor_details(op["outputs"][0])
            input_tensor = tensors[op["inputs"][0]]
            weights = tensors[op["inputs"][1]].transpose(1, 0)
            bias = tensors[op["inputs"][2]]
            output_shape_detail = interpreter._get_tensor_details(op["inputs"][0])
            output_shape_array = interpreter.get_tensor(output_shape_detail["index"])

            output_tensor = tf.keras.layers.Dense(
                units=output_shape_array.shape[3],
                use_bias=True,
                kernel_initializer=tf.keras.initializers.Constant(weights),
                bias_initializer=tf.keras.initializers.Constant(bias),
            )(input_tensor)
            tensors[output_detail["index"]] = output_tensor

        elif op_type == "RESIZE_BILINEAR":
            output_detail = interpreter._get_tensor_details(op["outputs"][0])
            input_tensor = tensors[op["inputs"][0]]
            size_detail = interpreter._get_tensor_details(op["inputs"][1])
            size = interpreter.get_tensor(size_detail["index"])
            size_height = size[0]
            size_width = size[1]

            def upsampling2d_bilinear(x, size_height, size_width):
                return tfv2.image.resize(
                    x, [size_height, size_width], method="bilinear"
                )

            output_tensor = tf.keras.layers.Lambda(
                upsampling2d_bilinear,
                arguments={"size_height": size_height, "size_width": size_width},
            )(input_tensor)
            tensors[output_detail["index"]] = output_tensor

        elif op_type == "DEQUANTIZE":
            output_detail = interpreter._get_tensor_details(op["outputs"][0])
            weights_detail = interpreter._get_tensor_details(op["inputs"][0])
            weights = interpreter.get_tensor(weights_detail["index"])
            output_tensor = weights.astype(np.float32)
            tensors[output_detail["index"]] = output_tensor

        else:
            raise ValueError(op_type)


def main():

    tf.disable_eager_execution()

    gen_model_json()
    ops, op_types = parse_json()

    interpreter = tf.lite.Interpreter(model_path)
    interpreter.allocate_tensors()
    input_details = interpreter.get_input_details()
    output_details = interpreter.get_output_details()
    print(input_details)
    print(output_details)

    make_graph(ops, op_types, interpreter)

    config = tf.ConfigProto()
    config.gpu_options.allow_growth = True
    graph = tf.get_default_graph()

    with tf.Session(config=config, graph=graph) as sess:
        sess.run(tf.global_variables_initializer())
        graph_def = tf.graph_util.convert_variables_to_constants(
            sess=sess,
            input_graph_def=graph.as_graph_def(),
            output_node_names=output_node_names,
        )

        shutil.rmtree(output_savedmodel_path, ignore_errors=True)
        tf.saved_model.simple_save(
            sess,
            output_savedmodel_path,
            inputs={"input": graph.get_tensor_by_name("input:0")},
            outputs={
                "classificators": graph.get_tensor_by_name("classificators:0"),
                "regressors": graph.get_tensor_by_name("regressors:0"),
            },
        )

if __name__ == "__main__":
    main()
