all:demo

CC=gcc
CXX=g++

SRC=$(shell find ./ -maxdepth 1 -name "*.cc")
OBJ := $(patsubst %.cc,%.o,${SRC})

CPPFLAGS = -I../inu/include -I/usr/include/opencv4
CXXFLAGS += -O2 -std=c++11
LDFLAGS += -L../inu/lib -L${OPENCV}/lib -lCommonUtilities -lInuStreams -lopencv_core -lopencv_highgui -lopencv_imgcodecs -lopencv_imgproc -lopencv_plot -lopencv_text -lstdc++

demo:${OBJ}
	${CC} $^ -o $@ ${LDFLAGS}

clean:
	-rm -rf ${OBJ}
	-rm demo
run: demo
	LD_LIBRARY_PATH=../inu/lib ./demo
