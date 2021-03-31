# README

## Prerequisite

  - pip install numpy

  - opencv
    
      - pip uninstall opencv-python
    
      - pip install opencv-python-headless
    
      - or build opencv from source in case you need `cv.imshow`
    
    `opencv-python` from pip was built with Qt support, which has
    `version mismatch` issue with pyqt5, we need to build `opencv` from
    source and remove Qt support
    
    https://forum.qt.io/topic/119109/using-pyqt5-with-opencv-python-cv2-causes-error-could-not-load-qt-platform-plugin-xcb-even-though-it-was-found
    
    https://docs.opencv.org/master/d7/d9f/tutorial\_linux\_install.html
    
    `cmake -D PYTHON_DEFAULT_EXECUTABLE=$(python -c "import sys;
    print(sys.executable)") -D PYTHON3_EXECUTABLE=$(python -c "import
    sys; print(sys.executable)") -D PYTHON3_NUMPY_INCLUDE_DIRS=$(python
    -c "import numpy; print (numpy.get_include())") -D
    PYTHON3_PACKAGES_PATH=$(python -c "from distutils.sysconfig import
    get_python_lib; print(get_python_lib())")
    -DOPENCV_EXTRA_MODULES_PATH=../opencv_contrib-master/modules
    ../opencv-master`

  - pip install pyqt5==5.15.2

  - pip install zmq

  - pip install scipy

  - conda install cudnn==8.0.4 -c nvidia

  - conda install cudatoolkit==11.1.74 -c nvidia

  - pip install onnxruntime-gpu==1.7.0

  - pip install onnx==1.8.1

  - pip install tensorflow==2.4.0

or using a fresh installed conda environment:

  - conda env create -f environment.yml

## Run

./launcher.sh

## Note

flops:

  - face detection: 52M
  - face landmark: 56M
  - iris landmark: 98M
  - palm detection: 198M
  - hand landmark: 710M
  - face recognition: 2700M
