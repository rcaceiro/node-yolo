#include "libyolo.h"
#include "private_structs.h"

#include <limits.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/time.h>
#include <unistd.h>

void fill_detect(yolo_object *yolo, detection *network_detection, int network_detection_index, detect *yolo_detect)
{
 size_t strlength=strlen(yolo->names[network_detection_index]);
 yolo->names[network_detection_index][strlength]='\0';
 yolo_detect->class_name=(char *)calloc(strlength+1, sizeof(char));
 strcpy(yolo_detect->class_name, yolo->names[network_detection_index]);
 yolo_detect->probability=network_detection->prob[network_detection_index]*100;
 yolo_detect->bbox=network_detection->bbox;
 box *bbox=&yolo_detect->bbox;
 bbox->x=bbox->x-(bbox->w/2);
 bbox->y=bbox->y-(bbox->h/2);
 if(bbox->x<0)
 {
  bbox->x=0;
 }
 if(bbox->y<0)
 {
  bbox->y=0;
 }
}

yolo_status fill_detection(yolo_object *yolo, detection *dets, yolo_detection_image *yolo_detect, float time_spent_for_classification, int nboxes, float thresh)
{
 yolo_detect->time_spent_for_classification=time_spent_for_classification;
 yolo_detect->num_boxes=0;
 int class_index;
 detection *det;

 for(int i=0; i<nboxes; ++i)
 {
  class_index=-1;
  det=nullptr;
  for(int j=0; j<dets[i].classes; ++j)
  {
   if(dets[i].prob[j]>=thresh)
   {
    if(det == nullptr || (dets[i].prob[j]>det->prob[class_index]))
    {
     class_index=j;
     det=dets+i;
    }
   }
  }
  if(class_index>-1 && det != nullptr)
  {
   void *temp_pointer=realloc(yolo_detect->detection, sizeof(detect)*(yolo_detect->num_boxes+1));
   if(temp_pointer == nullptr)
   {
    return yolo_cannot_realloc_detect;
   }
   yolo_detect->detection=(detect *)temp_pointer;
   fill_detect(yolo, det, class_index, yolo_detect->detection+yolo_detect->num_boxes);
   yolo_detect->num_boxes++;
  }
 }

 return yolo_ok;
}

yolo_status parse_detections_image(yolo_object *yolo, detection *dets, yolo_detection_image **yolo_detect, float time_spent_for_classification, int nboxes, float thresh)
{
 if((*yolo_detect) == nullptr)
 {
  (*yolo_detect)=(yolo_detection_image *)calloc(1, sizeof(yolo_detection_image));
  if((*yolo_detect) == nullptr)
  {
   return yolo_cannot_alloc_yolo_detection;
  }
 }

 return fill_detection(yolo, dets, (*yolo_detect), time_spent_for_classification, nboxes, thresh);
}

yolo_status parse_detections_video(yolo_object *yolo, detection *dets, yolo_detection_video **yolo_detect, float time_spent_for_classification, long frame_id, double milisecond, int nboxes, float thresh)
{
 if((*yolo_detect) == nullptr)
 {
  (*yolo_detect)=(yolo_detection_video *)calloc(1, sizeof(yolo_detection_video *));
  if((*yolo_detect) == nullptr)
  {
   return yolo_cannot_alloc_yolo_detection;
  }
 }
 yolo_detection_video *video_detection=*yolo_detect;
 auto *temp=(yolo_detection_frame *)realloc(video_detection->frame_detections, sizeof(yolo_detection_frame)*(video_detection->count+1));
 if(temp == nullptr)
 {
  return yolo_cannot_alloc_yolo_detection;
 }
 memset(temp+video_detection->count, 0, sizeof(yolo_detection_frame));
 video_detection->frame_detections=temp;
 video_detection->frame_detections[video_detection->count].frame=frame_id;
 video_detection->frame_detections[video_detection->count].milisecond=milisecond;
 yolo_status yolo_stats=fill_detection(yolo, dets, &video_detection->frame_detections[video_detection->count].detection_frame, time_spent_for_classification, nboxes, thresh);
 ++video_detection->count;
 return yolo_stats;
}

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

unsigned long long unixTimeMilis()
{
 struct timeval tv{};

 gettimeofday(&tv, nullptr);

 return (unsigned long long)(tv.tv_sec)*1000+(unsigned long long)(tv.tv_usec)/1000;
}

void *thread_capture(void *data)
{
 bool first_time_wait_push_image=true;

 if(data == nullptr)
 {
  return nullptr;
 }
 auto *thread_data=(thread_get_frame_t *)data;
 cv::Mat mat;

 while(true)
 {
  if(!thread_data->video->isOpened())
  {
   thread_data->image_queue->common->end=true;
   break;
  }
  queue_image_t queue_image;
  if(pthread_mutex_lock(&thread_data->mutex))
  {
   continue;
  }
  unsigned long long int startTime=unixTimeMilis();

  (*thread_data->video)>>mat;
  queue_image.milisecond=thread_data->video->get(CV_CAP_PROP_POS_MSEC);
  queue_image.frame_number=(long)thread_data->video->get(CV_CAP_PROP_POS_FRAMES);

  pthread_mutex_unlock(&thread_data->mutex);

  if(mat.empty())
  {
   thread_data->image_queue->common->end=true;
   break;
  }
  image yolo_image=libyolo_mat_to_image(mat);
  mat.release();

  queue_image.frame=yolo_image;

  unsigned long long int time=(unixTimeMilis()-startTime);

  if(sem_trywait(thread_data->image_queue->empty))
  {
   if(first_time_wait_push_image)
   {
    first_time_wait_push_image=false;
   }
  }

  if(!first_time_wait_push_image)
  {
   sem_wait(thread_data->image_queue->empty);
  }

  if(pthread_mutex_lock(&thread_data->image_queue->mutex))
  {
   sem_post(thread_data->image_queue->empty);
   continue;
  }
  thread_data->image_queue->queue.push_back(queue_image);
  pthread_mutex_unlock(&thread_data->image_queue->mutex);
  sem_post(thread_data->image_queue->full);

  pthread_mutex_lock(&thread_data->mutex);
  thread_data->number_of_samples++;
  thread_data->total_milis+=time;
  if(!first_time_wait_push_image)
  {
   thread_data->number_of_wait_push_image++;
   first_time_wait_push_image=true;
  }
  pthread_mutex_unlock(&thread_data->mutex);
 }
 return nullptr;
}

void *thread_detect(void *data)
{
 bool first_time_wait_pop_image=true;
 if(data == nullptr)
 {
  return nullptr;
 }
 auto *th_data=(thread_processing_image_t *)data;
 th_data->number_of_wait_pop_get_image=0;
 th_data->total_milis=0;
 th_data->number_of_samples=0;
 while(true)
 {
  if(sem_trywait(th_data->image_queue->full))
  {
   if(th_data->image_queue->common->end)
   {
    break;
   }
   if(first_time_wait_pop_image)
   {
    first_time_wait_pop_image=false;
    th_data->number_of_wait_pop_get_image++;
   }
   continue;
  }

  queue_image_t queue_image;
  bool im_got_sucessfull;
  if(pthread_mutex_lock(&th_data->image_queue->mutex))
  {
   continue;
  }
  im_got_sucessfull=!th_data->image_queue->queue.empty();
  if(im_got_sucessfull)
  {
   queue_image=th_data->image_queue->queue.front();
   th_data->image_queue->queue.pop_front();
  }
  pthread_mutex_unlock(&th_data->image_queue->mutex);
  sem_post(th_data->image_queue->empty);

  first_time_wait_pop_image=true;
  unsigned long long int startTime=unixTimeMilis();

  if(!im_got_sucessfull)
  {
   continue;
  }

  layer l=th_data->yolo->net->layers[th_data->yolo->net->n-1];
  unsigned long long time;
  float nms=0.45;

  image sized=resize_image(queue_image.frame, th_data->yolo->net->w, th_data->yolo->net->h);
  float *X=sized.data;
  time=unixTimeMilis();
  network_predict(th_data->yolo->net, X);

  int nboxes=0;
  detection *dets=get_network_boxes(th_data->yolo->net, queue_image.frame.w, queue_image.frame.h, th_data->thresh, 0, nullptr, 0, &nboxes);
  if(nms>0)
  {
   do_nms_sort(dets, l.side*l.side*l.n, l.classes, nms);
  }

  parse_detections_video(th_data->yolo, dets, th_data->yolo_detect, (unixTimeMilis()-time), queue_image.frame_number, queue_image.milisecond, nboxes, th_data->thresh);
  free_detections(dets, nboxes);

  th_data->total_milis+=(unixTimeMilis()-startTime);

  free_image(queue_image.frame);
  free_image(sized);

  th_data->number_of_samples++;
 }
 return nullptr;
}

yolo_status yolo_check_before_process_filename(yolo_object *yolo, char *filename)
{
 if(yolo == nullptr)
 {
  return yolo_object_is_not_initialized;
 }

 if(access(filename, F_OK) == -1)
 {
  fprintf(stderr, "error yolo_detect: %s\n", strerror(errno));
  return yolo_image_file_is_not_exists;
 }

 if(access(filename, R_OK) == -1)
 {
  fprintf(stderr, "error yolo_detect: %s\n", strerror(errno));
  return yolo_image_file_is_not_readable;
 }
 return yolo_ok;
}

void yolo_cleanup(yolo_object *yolo)
{
 if(yolo == nullptr)
 {
  return;
 }

 if(yolo->net != nullptr)
 {
  free_network(yolo->net);
 }

 if(yolo->names != nullptr)
 {
  for(int i=0; i<yolo->class_number; i++)
  {
   if(yolo->names[i] != nullptr)
   {
    free(yolo->names[i]);
   }
  }
  free(yolo->names);
 }
 free(yolo);
 yolo_object **ptr_yolo=&yolo;
 (*ptr_yolo)=nullptr;
}

yolo_status yolo_init(yolo_object **yolo_obj, char *workingDir, char *datacfg, char *cfgfile, char *weightfile)
{
 clock_t time=clock();

 yolo_cleanup((*yolo_obj));

 (*yolo_obj)=(yolo_object *)malloc(sizeof(yolo_object));

 yolo_object *yolo=(*yolo_obj);
 if(!yolo)
 {
  return yolo_cannot_alloc_node_yolo_object;
 }
 memset(yolo, 0, sizeof(yolo_object));

 if(access(workingDir, F_OK) == -1)
 {
  fprintf(stderr, "error yolo_init: %s\n", strerror(errno));
  return yolo_working_dir_is_not_exists;
 }

 if(access(workingDir, R_OK) == -1)
 {
  fprintf(stderr, "error yolo_init: %s\n", strerror(errno));
  return yolo_working_dir_is_not_readable;
 }
 char cur_dir[1024];
 getcwd(cur_dir, sizeof(cur_dir));
 if(chdir(workingDir) == -1)
 {
  fprintf(stderr, "%s\n", strerror(errno));
  return yolo_cannot_change_to_working_dir;
 }

 if(access(cfgfile, F_OK) == -1)
 {
  fprintf(stderr, "error yolo_init: %s\n", strerror(errno));
  return yolo_cfgfile_is_not_exists;
 }
 if(access(cfgfile, R_OK) == -1)
 {
  fprintf(stderr, "error yolo_init: %s\n", strerror(errno));
  return yolo_cfgfile_is_not_readable;
 }
 if(access(weightfile, F_OK) == -1)
 {
  fprintf(stderr, "error yolo_init: %s\n", strerror(errno));
  return yolo_weight_file_is_not_exists;
 }
 if(access(weightfile, R_OK) == -1)
 {
  fprintf(stderr, "error yolo_init: %s\n", strerror(errno));
  return yolo_weight_file_is_not_readable;
 }
 yolo->net=load_network(cfgfile, weightfile, 0);

 if(access(datacfg, F_OK) == -1)
 {
  fprintf(stderr, "error yolo_init: %s\n", strerror(errno));
  return yolo_datacfg_is_not_exists;
 }

 if(access(datacfg, R_OK) == -1)
 {
  fprintf(stderr, "error yolo_init: %s\n", strerror(errno));
  return yolo_datacfg_is_not_readable;
 }

 list *options=read_data_cfg(datacfg);
 char *name_list=option_find_str(options, "names", "data/names.list");

 if(access(name_list, F_OK) == -1)
 {
  fprintf(stderr, "error yolo_init: %s\n", strerror(errno));
  return yolo_names_file_is_not_exists;
 }
 if(access(name_list, R_OK) == -1)
 {
  fprintf(stderr, "error yolo_init: %s\n", strerror(errno));
  return yolo_names_file_is_not_readable;
 }
 yolo->names=get_labels(name_list);
 char *classes=option_find_str(options, "classes", "data/names.list");
 char *bad_ptr=nullptr;
 long value=strtol(classes, &bad_ptr, 10);
 if(value<INT_MAX)
 {
  yolo->class_number=(int)value;
 }

 set_batch_network(yolo->net, 1);
 srand(2222222);

 printf("Network configured and loaded in %f seconds\n", sec(clock()-time));
 chdir(cur_dir);
 return yolo_ok;
}

yolo_status yolo_detect_image(yolo_object *yolo, yolo_detection_image **detect, char *filename, float thresh)
{
 yolo_status status=yolo_check_before_process_filename(yolo, filename);
 if(status != yolo_ok)
 {
  return status;
 }

 cv::Mat mat=cv::imread(filename, CV_LOAD_IMAGE_COLOR);
 if(mat.empty())
 {
  fprintf(stderr, "error yolo_detect: %s\n", strerror(errno));
  return yolo_image_file_is_corrupted;
 }
 mat.release();

 layer l=yolo->net->layers[yolo->net->n-1];
 unsigned long long time;
 float nms=0.45;

 image im=load_image_color(filename, 0, 0);
 image sized=resize_image(im, yolo->net->w, yolo->net->h);
 float *X=sized.data;
 time=unixTimeMilis();
 network_predict(yolo->net, X);

 int nboxes=0;
 detection *dets=get_network_boxes(yolo->net, im.w, im.h, thresh, 0.5, nullptr, 0, &nboxes);
 if(nms>0)
 {
  do_nms_sort(dets, l.side*l.side*l.n, l.classes, nms);
 }

 status=parse_detections_image(yolo, dets, detect, unixTimeMilis()-time, nboxes, thresh);
 if(status != yolo_ok)
 {
  return status;
 }

 free_detections(dets, nboxes);
 free_image(im);
 free_image(sized);

 return yolo_ok;
}

yolo_status yolo_detect_video(yolo_object *yolo, yolo_detection_video **detect, char *filename, float thresh, unsigned int percentage_frames_to_drop)
{
 unsigned long long start=unixTimeMilis();
 yolo_status status=yolo_check_before_process_filename(yolo, filename);
 if(status != yolo_ok)
 {
  return status;
 }
 const size_t num_capture_image_threads=2;
 pthread_t *capture_image_thread;
 pthread_t process_image_thread;
 cv::VideoCapture *capture;

 thread_image_queue_t image_queue;
 thread_common_t data_image_common;
 thread_get_frame_t data_get_image;
 thread_processing_image_t data_process_image;

 data_image_common.end=false;
 image_queue.queue=std::deque<queue_image_t>();
 image_queue.common=&data_image_common;

 data_get_image.image_queue=data_process_image.image_queue=&image_queue;

 data_get_image.percentage_frames_to_drop=percentage_frames_to_drop;

 data_process_image.yolo=yolo;
 data_process_image.thresh=thresh;
 data_process_image.yolo_detect=detect;

 //TEMP////////////////////////////////////////////////////////////////
 data_get_image.total_milis=0;
 data_get_image.number_of_samples=0;
 data_get_image.number_of_wait_push_image=0;
 if(pthread_mutex_init(&data_get_image.mutex, nullptr))
 {
  return yolo_video_cannot_alloc_base_structure;
 }

 data_process_image.number_of_wait_pop_get_image=0;
 //////////////////////////////////////////////////////////////////////////
 if(pthread_mutex_init(&image_queue.mutex, nullptr))
 {
  return yolo_video_cannot_alloc_base_structure;
 }

 image_queue.empty=sem_open("/image_empty", O_CREAT, 0644, 20);
 if(image_queue.empty == SEM_FAILED)
 {
  return yolo_video_cannot_alloc_base_structure;
 }
 image_queue.full=sem_open("/image_full", O_CREAT, 0644, 0);
 if(image_queue.full == SEM_FAILED)
 {
  return yolo_video_cannot_alloc_base_structure;
 }

 capture=new cv::VideoCapture(filename);
 if(!capture->isOpened())
 {
  return yolo_cannot_open_video_stream;
 }
 data_get_image.video=capture;

 capture_image_thread=(pthread_t *)calloc(num_capture_image_threads, sizeof(pthread_t));
 if(capture_image_thread == nullptr)
 {
  return yolo_video_cannot_alloc_base_structure;
 }

 for(size_t i=0; i<num_capture_image_threads; ++i)
 {
  pthread_create(capture_image_thread+i, nullptr, thread_capture, &data_get_image);
 }
 pthread_create(&process_image_thread, nullptr, thread_detect, &data_process_image);

 for(size_t i=0; i<num_capture_image_threads; ++i)
 {
  pthread_join(capture_image_thread[i], nullptr);
 }
 capture->release();
 delete capture;
 free(capture_image_thread);

 pthread_join(process_image_thread, nullptr);

 sem_close(image_queue.full);
 sem_close(image_queue.empty);
 pthread_mutex_destroy(&image_queue.mutex);
 image_queue.queue.clear();

 //TEMP///////////////////////////////////////////////////////
 pthread_mutex_destroy(&data_get_image.mutex);
 ///////////////////////////////////////////////////////////

 printf("Process video took %llu\n", unixTimeMilis()-start);
 printf("Process get frames took around %lf and the number of waits to push the image object was %lu\n", data_get_image.total_milis/(1.0f*data_get_image.number_of_samples), data_get_image.number_of_wait_push_image);
 printf("Process %lu images took around %lf and the number of waits to pop the image object was %lu\n", data_get_image.number_of_samples, data_process_image.total_milis/(1.0f*data_process_image.number_of_samples), data_process_image.number_of_wait_pop_get_image);
 return yolo_ok;
}

void yolo_detect_free(yolo_detection_image *yolo_det)
{
 for(size_t i=0; i<yolo_det->num_boxes; i++)
 {
  free(yolo_det->detection[i].class_name);
 }
 free(yolo_det->detection);
}

void yolo_detection_image_free(yolo_detection_image **yolo)
{
 if((*yolo) == nullptr)
 {
  return;
 }
 yolo_detect_free(*yolo);
 free(*yolo);
 (*yolo)=nullptr;
}

void yolo_detection_video_free(yolo_detection_video **yolo)
{
 yolo_detection_video *yolo_det=*yolo;
 if(yolo_det == nullptr)
 {
  return;
 }
 for(size_t i=0; i<yolo_det->count; ++i)
 {
  yolo_detect_free(&yolo_det->frame_detections[i].detection_frame);
 }
 free(yolo_det->frame_detections);
 free(yolo_det);
 (*yolo)=nullptr;
}