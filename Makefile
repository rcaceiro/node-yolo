GPU=0
CUDNN=0
OPENCV=0
OPENMP=0
DEBUG=0

ARCH= -gencode arch=compute_20,code=[sm_20,sm_21] \
      -gencode arch=compute_30,code=sm_30 \
      -gencode arch=compute_35,code=sm_35 \
      -gencode arch=compute_50,code=[sm_50,compute_50] \
      -gencode arch=compute_52,code=[sm_52,compute_52]

# This is what I use, uncomment if you know your arch and want to specify
# ARCH= -gencode arch=compute_52,code=compute_52

VPATH=./darknet/src/
SHARE_LIB_OPT=

LIB_RAW_NAME=libyolo
SLIB=
SLIB_DIR=

ALIB=libyolo.a
ALIB_DIR=$(addprefix yolo/, $(ALIB))

OBJDIR=./obj/

CC=gcc
NVCC=nvcc
AR=ar
ARFLAGS=rcs
OPTS=
LDFLAGS= -lm -pthread 
COMMON= -I./darknet/include/ -I./darknet/src/
CFLAGS=-Wall -Wno-unknown-pragmas -Wfatal-errors -fPIC

UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
	SHARE_LIB_OPT += -shared
     SLIB=$(addprefix $(LIB_RAW_NAME),.so)
endif
ifeq ($(UNAME_S),Darwin)
	SHARE_LIB_OPT += -dynamiclib
     SLIB=$(addprefix $(LIB_RAW_NAME),.dylib)
endif

SLIB_DIR=$(addprefix yolo/, $(SLIB))

ifeq ($(OPENMP), 1) 
COMMON+= -fopenmp
endif

ifeq ($(DEBUG), 1) 
OPTS=-O0 -g
endif

CFLAGS+=$(OPTS)

ifeq ($(OPENCV), 1) 
COMMON+= -DOPENCV
CFLAGS+= -DOPENCV
LDFLAGS+= `pkg-config --libs opencv` 
COMMON+= `pkg-config --cflags opencv` 
endif

ifeq ($(GPU), 1) 
COMMON+= -DGPU -I/usr/local/cuda/include/
CFLAGS+= -DGPU
LDFLAGS+= -L/usr/local/cuda/lib64 -lcuda -lcudart -lcublas -lcurand
endif

ifeq ($(CUDNN), 1) 
COMMON+= -DCUDNN 
CFLAGS+= -DCUDNN
LDFLAGS+= -lcudnn
endif

OBJ=libyolo.o gemm.o utils.o cuda.o deconvolutional_layer.o convolutional_layer.o list.o image.o activations.o im2col.o col2im.o blas.o crop_layer.o dropout_layer.o maxpool_layer.o softmax_layer.o data.o matrix.o network.o connected_layer.o cost_layer.o parser.o option_list.o detection_layer.o route_layer.o box.o normalization_layer.o avgpool_layer.o layer.o local_layer.o shortcut_layer.o activation_layer.o rnn_layer.o gru_layer.o crnn_layer.o demo.o batchnorm_layer.o region_layer.o reorg_layer.o tree.o lstm_layer.o
EXECOBJA=captcha.o lsd.o super.o voxel.o art.o tag.o cifar.o go.o rnn.o rnn_vid.o compare.o segmenter.o regressor.o classifier.o coco.o dice.o yolo.o detector.o  writing.o nightmare.o swag.o darknet.o 
ifeq ($(GPU), 1) 
LDFLAGS+= -lstdc++ 
OBJ+=convolutional_kernels.o deconvolutional_kernels.o activation_kernels.o im2col_kernels.o col2im_kernels.o blas_kernels.o crop_layer_kernels.o dropout_layer_kernels.o maxpool_layer_kernels.o network_kernels.o avgpool_layer_kernels.o
endif

EXECOBJ = $(addprefix $(OBJDIR), $(EXECOBJA))
OBJS = $(addprefix $(OBJDIR), $(OBJ))
DEPS = $(wildcard src/*.h) Makefile ./darknet/include/darknet.h

all: obj $(SLIB_DIR) $(ALIB_DIR)

$(ALIB_DIR): $(OBJS)
	$(AR) $(ARFLAGS) $@ $^

$(SLIB_DIR): $(OBJS)
	$(CC) $(CFLAGS) $(SHARE_LIB_OPT) $^ -o $@ $(LDFLAGS)

$(OBJDIR)%.o: %.c $(DEPS)
	$(CC) $(COMMON) $(CFLAGS) -c $< -o $@

obj/libyolo.o: yolo/src/libyolo.c yolo/src/libyolo.h ./darknet/include/darknet.h
	$(CC) $(COMMON) $(CFLAGS) -c $< -o $@

$(OBJDIR)%.o: %.cu $(DEPS)
	$(NVCC) $(ARCH) $(COMMON) --compiler-options "$(CFLAGS)" -c $< -o $@

obj:
	mkdir -p obj

.PHONY: clean

clean_all: clean
	rm -rf $(SLIB_DIR) $(ALIB_DIR)

clean:
	rm -rf obj

install:
	cp $(SLIB_DIR) /usr/local/lib/$(SLIB)
	cp $(ALIB_DIR) /usr/local/lib/$(ALIB)
	mkdir -p /usr/local/include/yolo
	cp yolo/src/libyolo.h /usr/local/include/yolo/libyolo.h
	cp ./darknet/include/darknet.h /usr/local/include/yolo/darknet.h
	cp ./darknet/src/*.h /usr/local/include/yolo/

unistall:
	rm -rf /usr/local/include/yolo /usr/local/lib/$(SLIB) /usr/local/lib/$(ALIB)