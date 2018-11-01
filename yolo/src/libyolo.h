#ifndef LIBYOLO_H
#define LIBYOLO_H

#include "darknet.h"
#include "yolo_error.h"
#include <unistd.h>
#include <errno.h>
#include <semaphore.h>
#include <deque>
#include <opencv2/opencv.hpp>

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

typedef struct
{
 long frame_number;
 double milisecond;
 image frame;
}queue_image_t;

typedef struct
{
 long frame_number;
 double milisecond;
 float time_spent_for_classification;
 detection *frame_detections;
 int nboxes;
}queue_detection_t;

typedef struct
{
 pthread_mutex_t mutex_end;
 bool end;
}thread_common_t;

typedef struct
{
 sem_t *full;
 sem_t *empty;
 pthread_mutex_t mutex;
 std::deque<queue_image_t> queue;
}thread_image_queue_t;

typedef struct
{
 sem_t *full;
 sem_t *empty;
 pthread_mutex_t mutex;
 std::deque<queue_detection_t> queue;

 yolo_object *yolo;
 float thresh;
}thread_detections_queue_t;

typedef struct
{
 thread_common_t *common;
 thread_image_queue_t *image_queue;
 cv::VideoCapture *video;
}thread_get_frame_t;

typedef struct
{
 thread_common_t *common;
 thread_image_queue_t *image_queue;
 thread_detections_queue_t *detections_queue;
}thread_processing_image_t;

typedef struct
{
 thread_common_t *common;
 thread_detections_queue_t *detections_queue;

 yolo_detection_video **yolo_detect;
}thread_processing_detections_t;

yolo_status yolo_init(yolo_object **yolo_obj, char *workingDir, char *datacfg, char *cfgfile, char *weightfile);
yolo_status yolo_detect_image(yolo_object *yolo, yolo_detection_image **detect, char *filename, float thresh);
yolo_status yolo_detect_video(yolo_object *yolo, yolo_detection_video **detect, char *filename, float thresh);

void yolo_detection_image_free(yolo_detection_image **yolo);
void yolo_detection_video_free(yolo_detection_video **yolo);
void yolo_cleanup(yolo_object *yolo);

#ifdef __cplusplus
};
#endif

#endif // LIBYOLO_H