all:test.elf

CC=gcc
CXX=g++

SRC=$(shell find ./ -maxdepth 1 -name "*.cc")
OBJ := $(patsubst %.cc,%.o,${SRC})
-include $(OBJ:.o=.d)

NN_SRC=$(shell find ./tflite -name "*.cc")
NN_OBJ += $(patsubst %.cc,%.o,${NN_SRC})
-include $(NN_OBJ:.o=.d)

$(info ${NN_OBJ})

CPPFLAGS = -Iinu/include -I/usr/include/opencv4 ${TENSORFLOW_CPPFLAGS} -MMD

CXXFLAGS += -O2 -std=c++11 -fno-rtti -fomit-frame-pointer -Wall
LDFLAGS = -Linu/lib
LDLIBS += -lCommonUtilities -lInuStreams \
	-lopencv_core -lopencv_highgui -lopencv_imgcodecs -lopencv_imgproc -lopencv_plot -lopencv_text \
	-lstdc++ -lpthread -lm

TENSORFLOW_ROOT_DIR=./tflite
TENSORFLOW_CPPFLAGS=\
	-I. \
	-I${TENSORFLOW_ROOT_DIR}/tensorflow/lite/tools/make/downloads/flatbuffers/include \
	-I${TENSORFLOW_ROOT_DIR}/tensorflow/lite/tools/make/downloads/absl \
	-I${TENSORFLOW_ROOT_DIR} -MMD

tflite/%.o:tflite/%.cc
	${CXX} -c $< -o $@ ${TENSORFLOW_CPPFLAGS}

test.elf:${OBJ} ${NN_OBJ} tflite/libtensorflow-lite.a
	${CC} ${LDFLAGS} $^ -o $@ ${LDLIBS}

clean:
	-rm -rf ${OBJ}
	-rm $(NN_OBJ:.o=.d)
	-rm demo.elf
	-rm ${NN_OBJ}
	-rm $(NN_OBJ:.o=.d)

run: test.elf
	LD_LIBRARY_PATH=inu/lib test.elf
