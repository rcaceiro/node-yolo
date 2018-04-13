CMAKE_MINIMUM_REQUIRED(VERSION 3.10)
PROJECT(NodeYoloJS_)

SET(CMAKE_CXX_STANDARD 17)

INCLUDE_DIRECTORIES(.)
INCLUDE_DIRECTORIES(darknet/src)
INCLUDE_DIRECTORIES(darknet/include)
INCLUDE_DIRECTORIES(/usr/local/include/node)
INCLUDE_DIRECTORIES(./node_modules/node-addon-api)

ADD_EXECUTABLE(NodeYoloJS
			libyolo.h
			libyolo.c module.cpp module.cpp module.h)
