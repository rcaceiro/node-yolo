#ifndef LIBYOLO_H
#define LIBYOLO_H

#include "darknet.h"
#include <unistd.h>
#include <errno.h>

#ifdef __cplusplus
extern "C" {
#endif
typedef struct
{
 char *class_name;
 float probability;
 box bbox;
}detect;

typedef struct
{
 detect *detection;
 int num_boxes;
}yolo_detection;

typedef struct
{
 int class_number;
 char **names;
 float nms;
 network *net;
}yolo_object;

yolo_object *yolo_init(char *workingDir, char *datacfg, char *cfgfile, char *weightfile);
void yolo_cleanup(yolo_object *yolo);
yolo_detection *yolo_detect(yolo_object *yolo, char *filename, float thresh);
void yolo_detection_free(yolo_detection **yolo);
#ifdef __cplusplus
};
#endif

#endif // LIBYOLO_H