#ifndef NODE_YOLO_PRIVATE_STRUCTS_H
#define NODE_YOLO_PRIVATE_STRUCTS_H

#include "libyolo.h"

#include <semaphore.h>

#include <deque>
#include <opencv2/opencv.hpp>

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

 unsigned long long int total_milis;
 unsigned long int number_of_samples;
 pthread_mutex_t mutex;
}thread_get_frame_t;

typedef struct
{
 thread_common_t *common;
 thread_image_queue_t *image_queue;
 thread_detections_queue_t *detections_queue;

 unsigned long long int total_milis;
 unsigned long int number_of_samples;
}thread_processing_image_t;

typedef struct
{
 thread_common_t *common;
 thread_detections_queue_t *detections_queue;

 yolo_detection_video **yolo_detect;

 unsigned long long int total_milis;
 unsigned long int number_of_samples;
 pthread_mutex_t mutex;
}thread_processing_detections_t;

#endif //NODE_YOLO_PRIVATE_STRUCTS_H