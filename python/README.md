README
======

Prerequisite
------------

-   pip uninstall opencv-python
-   pip install opencv-python-headless
-   pip install pyqt5==5.15.2
-   pip install zmq
-   pip install scipy

for tensorflow
--------------

-   conda install cudnn==7.6.5
-   pip install tensorflow-gpu==2.2.0

for onnx
--------

-   conda install cudnn==8.0.4
-   pip install onnxruntime-gpu==1.7.0
-   pip install onnx==1.8.1

Run
---

./launcher.sh

Note
---

flops:

face detection:       52M
face landmark:        56M
iris landmark:        98M
palm detection:       198M
hand landmark:        710M
face recognition:     2700M
