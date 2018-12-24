CMAKE_MINIMUM_REQUIRED(VERSION 3.10)
PROJECT(node-yolo)

SET(CMAKE_CXX_STANDARD 17)

INCLUDE_DIRECTORIES(.)
INCLUDE_DIRECTORIES(darknet/src)
INCLUDE_DIRECTORIES(darknet/include)
INCLUDE_DIRECTORIES(yolo/src)
INCLUDE_DIRECTORIES(/usr/local/include/node)
INCLUDE_DIRECTORIES(/usr/include/node)
INCLUDE_DIRECTORIES(/usr/local/Cellar/opencv/3.4.3_1/include/opencv)
INCLUDE_DIRECTORIES(/usr/local/Cellar/opencv/3.4.3_1/include)
ADD_COMPILE_DEFINITIONS(OPENCV)

ADD_EXECUTABLE(node-yolo
			yolo/src/libyolo.h
			yolo/src/libyolo.cpp src/module.cpp src/module.h yolo/src/yolo_error.c yolo/src/yolo_error.h yolo/src/private_structs.h src/napi_yolo_errors.c src/napi_yolo_errors.h)
