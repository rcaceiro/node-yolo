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
 detection *detection;
 int num_boxes;
}yolo_detection;

typedef struct
{
 char **names;
 float nms;
 network *net;
}yolo_object;

yolo_object *yolo_init(char *datacfg, char *cfgfile, char *weightfile);

void yolo_cleanup(yolo_object *yolo);

yolo_detection *yolo_detect(network *net, char *filename, float thresh);

void yolo_detection_free(yolo_detection **yolo);

#ifdef __cplusplus
};
#endif

#endif // LIBYOLO_H