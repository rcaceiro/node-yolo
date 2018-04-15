#ifndef LIBYOLO_H
#define LIBYOLO_H

#include "darknet.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
 detection *detection;
 int num_boxes;
}yolo_detection;

network *yolo_init(char *darknet_path, char *datacfg, char *cfgfile, char *weightfile);

void yolo_cleanup(network *net);

yolo_detection *yolo_detect(network *net, image im, float thresh, float hier_thresh, int *num);
#ifdef __cplusplus
}
#endif

#endif // LIBYOLO_H