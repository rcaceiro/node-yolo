#include <opencv2/opencv.hpp>
#include "darknet.h"
extern "C" {
#include "libyolo.h"
#include "stack.h"
#include <fcntl.h>

image libyolo_ipl_to_image(IplImage *src)
{
 int h=src->height;
 int w=src->width;
 int c=src->nChannels;
 image im=make_image(w, h, c);
 auto *data=(unsigned char *)src->imageData;
 int step=src->widthStep;
 int i, j, k;

 for(i=0; i<h; ++i)
 {
  for(k=0; k<c; ++k)
  {
   for(j=0; j<w; ++j)
   {
    im.data[k*w*h+i*w+j]=data[i*step+j*c+k]/255.;
   }
  }
 }
 return im;
}

image libyolo_mat_to_image(cv::Mat &m)
{
 IplImage ipl=m;
 image im=libyolo_ipl_to_image(&ipl);
 rgbgr_image(im);
 return im;
}

void *thread_capture(void *data)
{
 if(data == nullptr)
 {
  return nullptr;
 }
 auto *thread_data=static_cast<thread_data_t *>(data);
 cv::Mat mat;
 auto *cap=(cv::VideoCapture *)thread_data->video;

 while(true)
 {
  if(!cap->isOpened())
  {
   pthread_mutex_lock(&thread_data->mutex_end);
   thread_data->end=true;
   pthread_mutex_unlock(&thread_data->mutex_end);
   break;
  }

  (*cap)>>mat;
  if(mat.empty())
  {
   pthread_mutex_lock(&thread_data->mutex_end);
   thread_data->end=true;
   pthread_mutex_unlock(&thread_data->mutex_end);
   break;
  }
  image yolo_image=libyolo_mat_to_image(mat);
  sem_wait(thread_data->empty);
  if(pthread_mutex_lock(&thread_data->mutex_stack))
  {
   sem_post(thread_data->empty);
   continue;
  }
  stack_push(&thread_data->stack,yolo_image,(long)cap->get(CV_CAP_PROP_POS_FRAMES),cap->get(CV_CAP_PROP_POS_MSEC));
  pthread_mutex_unlock(&thread_data->mutex_stack);
  sem_post(thread_data->full);
 }
 return nullptr;
}

yolo_status yolo_detect_video(yolo_object *yolo, yolo_detection_video **detect, char *filename, float thresh)
{
 yolo_status status=yolo_check_before_process_filename(yolo, filename);
 if(status != yolo_ok)
 {
  return status;
 }

 pthread_t consumer;
 pthread_t producer;
 cv::VideoCapture *capture;
 thread_data_t *thread_data=nullptr;
 thread_data=static_cast<thread_data_t *>(calloc(1, sizeof(thread_data_t)));
 if(thread_data == nullptr)
 {
  return yolo_video_cannot_alloc_base_structure;
 }
 thread_data->yolo_detect=detect;
 thread_data->yolo=yolo;
 thread_data->end=false;
 thread_data->thresh=thresh;
 if(pthread_mutex_init(&thread_data->mutex_stack, nullptr))
 {
  return yolo_video_cannot_alloc_base_structure;
 }
 if(pthread_mutex_init(&thread_data->mutex_end, nullptr))
 {
  return yolo_video_cannot_alloc_base_structure;
 }
 thread_data->empty=sem_open("/empty", O_CREAT, 0644, 10);
 if(thread_data->empty == SEM_FAILED)
 {
  return yolo_video_cannot_alloc_base_structure;
 }
 thread_data->full=sem_open("/full", O_CREAT, 0644, 0);
 if(thread_data->full == SEM_FAILED)
 {
  return yolo_video_cannot_alloc_base_structure;
 }

 capture=new cv::VideoCapture(filename);
 if(!capture->isOpened())
 {
  return yolo_cannot_open_video_stream;
 }
 thread_data->video=capture;

 if(pthread_create(&producer, nullptr, thread_capture, thread_data))
 {
  return yolo_video_cannot_alloc_base_structure;
 }
 if(pthread_create(&consumer, nullptr, thread_detect, thread_data))
 {
  return yolo_video_cannot_alloc_base_structure;
 }

 pthread_join(producer, nullptr);
 capture->release();
 delete capture;

 pthread_join(consumer, nullptr);
 sem_close(thread_data->full);
 sem_close(thread_data->empty);
 pthread_mutex_destroy(&thread_data->mutex_stack);
 pthread_mutex_destroy(&thread_data->mutex_end);
 free(thread_data);

 return yolo_ok;
}
};