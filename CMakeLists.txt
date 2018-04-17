CMAKE_MINIMUM_REQUIRED(VERSION 3.10)
PROJECT(NodeYoloJS_)

SET(CMAKE_CXX_STANDARD 17)

INCLUDE_DIRECTORIES(.)
INCLUDE_DIRECTORIES(darknet/src)
INCLUDE_DIRECTORIES(darknet/include)
INCLUDE_DIRECTORIES(yolo/src)
INCLUDE_DIRECTORIES(/usr/local/include/node)

ADD_EXECUTABLE(NodeYoloJS
			yolo/src/libyolo.h
			yolo/src/libyolo.c src/module.cpp src/module.cpp src/module.h)
