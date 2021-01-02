all:demo.elf

CC=gcc
CXX=g++

SRC=$(shell find ./ -maxdepth 1 -name "*.cc")
OBJ := $(patsubst %.cc,%.o,${SRC})

TFLITE_SRC=$(shell find ./tflite -name "*.cc" -or -name "*.c")
TFLITE_OBJ := $(patsubst %.c,%.o,$(filter %.c, $(TFLITE_SRC)))
TFLITE_OBJ += $(patsubst %.cc,%.o,$(filter %.cc, $(TFLITE_SRC)))

CPPFLAGS = -Iinu/include -I/usr/include/opencv4
CPPFLAGS += -I. -Itflite -Itflite/third_party/flatbuffers/include -Itflite/third_party/ruy -Itflite/third_party/gemmlowp

CXXFLAGS += -O2 -std=c++11 -fno-rtti -fomit-frame-pointer -Wall
LDFLAGS = -Linu/lib
LDLIBS += -lCommonUtilities -lInuStreams \
	-lopencv_core -lopencv_highgui -lopencv_imgcodecs -lopencv_imgproc -lopencv_plot -lopencv_text \
	-lstdc++

demo.elf:${OBJ}
	${CC} ${LDFLAGS} $^ -o $@ ${LDLIBS}

clean:
	-rm -rf ${OBJ}
	-rm demo.elf

run: demo.elf
	LD_LIBRARY_PATH=inu/lib ./demo.elf
