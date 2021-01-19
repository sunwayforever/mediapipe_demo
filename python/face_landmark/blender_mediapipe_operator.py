#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# 2021-01-18 16:59
import bpy
import cv2
import time
import numpy as np

import sys

sys.path.append("/home/sunway/source/mediapipe-demo/python")

from face_landmark.facemesh import mesh_generator


class MediapipeOperator(bpy.types.Operator):
    bl_idname = "wm.mediapipe_operator"
    bl_label = "Mediapipe Animation Operator"

    _timer = None
    stop = False

    width = 640
    height = 480

    _stream = mesh_generator()

    def smooth_value(self, name, length, value):
        if not hasattr(self, "smooth"):
            self.smooth = {}
        if not name in self.smooth:
            self.smooth[name] = np.array([value])
        else:
            self.smooth[name] = np.insert(arr=self.smooth[name], obj=0, values=value)
            if self.smooth[name].size > length:
                self.smooth[name] = np.delete(
                    self.smooth[name], self.smooth[name].size - 1, 0
                )
        sum = 0
        for val in self.smooth[name]:
            sum += val
        return sum / self.smooth[name].size

    # Keeps min and max values, then returns the value in a range 0 - 1
    def get_range(self, name, value):
        if not hasattr(self, "range"):
            self.range = {}
        if not name in self.range:
            self.range[name] = np.array([value, value])
        else:
            self.range[name] = np.array(
                [min(value, self.range[name][0]), max(value, self.range[name][1])]
            )
            val_range = self.range[name][1] - self.range[name][0]
        if val_range != 0:
            return (value - self.range[name][0]) / val_range
        else:
            return 0.0

    def modal(self, context, event):
        if (event.type in {"RIGHTMOUSE", "ESC"}) or self.stop == True:
            self.cancel(context)
            return {"CANCELLED"}

        if event.type == "TIMER":
            rotation_vector, _ = next(self._stream)
            # set bone rotation/positions
            bones = bpy.data.objects["RIG-Vincent"].pose.bones
            # yaw
            bones["head_fk"].rotation_euler[1] = self.smooth_value(
                "h_x", 5, -rotation_vector[0]
            )
            # pitch
            bones["head_fk"].rotation_euler[0] = self.smooth_value(
                "h_y", 5, rotation_vector[1] - 0.2
            )
            # roll
            bones["head_fk"].rotation_euler[2] = self.smooth_value(
                "h_z", 5, np.pi - rotation_vector[2]
            )

            bones["head_fk"].keyframe_insert(data_path="rotation_euler", index=-1)

            # # mouth position
            # bones["mouth_ctrl"].location[2] = self.smooth_value(
            #     "m_h",
            #     2,
            #     -self.get_range(
            #         "mouth_height", np.linalg.norm(shape[62] - shape[66])
            #     )
            #     * 0.06,
            # )
            # bones["mouth_ctrl"].location[0] = self.smooth_value(
            #     "m_w",
            #     2,
            #     (
            #         self.get_range(
            #             "mouth_width", np.linalg.norm(shape[54] - shape[48])
            #         )
            #         - 0.5
            #     )
            #     * -0.04,
            # )
            # bones["mouth_ctrl"].keyframe_insert(data_path="location", index=-1)

            cv2.waitKey(1)

        return {"PASS_THROUGH"}

    def stop_playback(self, scene):
        print(format(scene.frame_current) + " / " + format(scene.frame_end))
        if scene.frame_current == scene.frame_end:
            bpy.ops.screen.animation_cancel(restore_frame=False)

    def execute(self, context):
        bpy.app.handlers.frame_change_pre.append(self.stop_playback)

        wm = context.window_manager
        self._timer = wm.event_timer_add(0.01, window=context.window)
        wm.modal_handler_add(self)
        return {"RUNNING_MODAL"}

    def cancel(self, context):
        wm = context.window_manager
        wm.event_timer_remove(self._timer)
        cv2.destroyAllWindows()


def register():
    bpy.utils.register_class(MediapipeOperator)


def unregister():
    bpy.utils.unregister_class(MediapipeOperator)


if __name__ == "__main__":
    register()
    bpy.ops.wm.mediapipe_operator()
