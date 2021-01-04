all:demo.elf

CC=gcc
CXX=g++

SRC=$(shell find ./ -maxdepth 1 -name "*.cc")
OBJ := $(patsubst %.cc,%.o,${SRC})

NN_SRC=$(shell find ./tflite -name "*.cc")
NN_OBJ += $(patsubst %.cc,%.o,${NN_SRC})

$(info ${NN_OBJ})

CPPFLAGS = -Iinu/include -I/usr/include/opencv4

CXXFLAGS += -O2 -std=c++11 -fno-rtti -fomit-frame-pointer -Wall
LDFLAGS = -Linu/lib
LDLIBS += -lCommonUtilities -lInuStreams \
	-lopencv_core -lopencv_highgui -lopencv_imgcodecs -lopencv_imgproc -lopencv_plot -lopencv_text \
	-lstdc++

TENSORFLOW_ROOT_DIR=./tflite
TENSORFLOW_CPPFLAGS=\
	-I${TENSORFLOW_ROOT_DIR}/tensorflow/lite/tools/make/downloads/flatbuffers/include \
	-I${TENSORFLOW_ROOT_DIR}/tensorflow/lite/tools/make/downloads/absl \
	-I${TENSORFLOW_ROOT_DIR}

tflite/%.o:tflite/%.cc
	${CXX} -c $^ -o $@ ${TENSORFLOW_CPPFLAGS}

tflite/libnn.a: ${NN_OBJ}
	ar rcs $@ $^

test.elf:${OBJ}
	${CC} ${LDFLAGS} $^ -o $@ ${LDLIBS}

clean:
	-rm -rf ${OBJ}
	-rm demo.elf
	-rm tflite/libnn.a
	-rm ${NN_OBJ}

run: test.elf
	LD_LIBRARY_PATH=inu/lib test.elf
