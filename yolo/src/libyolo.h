#ifndef LIBYOLO_H
#define LIBYOLO_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "darknet.h"
#include "option_list.h"
#include "image.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void *yolo_handle;

typedef struct
{
 char name[32];
 int left;
 int right;
 int top;
 int bottom;
 float prob;
}detection_info;

yolo_handle yolo_init(char *darknet_path, char *datacfg, char *cfgfile, char *weightfile);

void yolo_cleanup(yolo_handle handle);

detection_info **yolo_detect(yolo_handle handle, image im, float thresh, float hier_thresh, int *num);

detection_info **yolo_test(yolo_handle handle, char *filename, float thresh, float hier_thresh, int *num, float **feature_map, int *map_size);

#ifdef __cplusplus
}
#endif

#endif // LIBYOLO_H