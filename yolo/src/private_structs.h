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
 bool end;
}thread_common_t;

typedef struct
{
 sem_t *full;
 sem_t *empty;
 pthread_mutex_t mutex;
 std::deque<queue_image_t> queue;

 thread_common_t *common;
}thread_image_queue_t;

typedef struct
{
 thread_image_queue_t *image_queue;
 cv::VideoCapture *video;

 unsigned int number_frames_to_process_simultaneously;
 unsigned int number_frames_to_process;
 unsigned long long int total_milis;
 unsigned long int number_of_samples;
 unsigned long int number_of_wait_push_image;
 pthread_mutex_t mutex;
}thread_get_frame_t;

typedef struct
{
 thread_image_queue_t *image_queue;

 yolo_object *yolo;
 float thresh;
 yolo_detection_video **yolo_detect;

 unsigned long long int total_milis;
 unsigned long int number_of_samples;
 unsigned long int number_of_wait_pop_get_image;
}thread_processing_image_t;

#endif //NODE_YOLO_PRIVATE_STRUCTS_H