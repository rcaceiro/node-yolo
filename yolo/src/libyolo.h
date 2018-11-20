#ifndef LIBYOLO_H
#define LIBYOLO_H

#include "darknet.h"
#include "yolo_error.h"

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
 size_t num_boxes;
 float time_spent_for_classification;
}yolo_detection_image;

typedef struct
{
 yolo_detection_image detection_frame;
 double milisecond;
 long frame;
}yolo_detection_frame;

typedef struct
{
 yolo_detection_frame *frame_detections;
 size_t count;
}yolo_detection_video;

typedef struct
{
 int class_number;
 char **names;
 float nms;
 network *net;
}yolo_object;

yolo_status yolo_init(yolo_object **yolo_obj, char *workingDir, char *datacfg, char *cfgfile, char *weightfile);
yolo_status yolo_detect_image(yolo_object *yolo, yolo_detection_image **detect, char *filename, float thresh);
yolo_status yolo_detect_video(yolo_object *yolo, yolo_detection_video **detect, char *filename, float thresh, double fraction_frames_to_drop);

void yolo_detection_image_free(yolo_detection_image **yolo);
void yolo_detection_video_free(yolo_detection_video **yolo);
void yolo_cleanup(yolo_object *yolo);

#ifdef __cplusplus
};
#endif

#endif // LIBYOLO_H