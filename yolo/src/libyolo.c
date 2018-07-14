#include "libyolo.h"
#include "map_lib.h"
#include <limits.h>

void yolo_cleanup(yolo_object *yolo)
{
 free_network(yolo->net);
 for(int i=0; i<yolo->class_number; i++)
 {
  free(yolo->names[i]);
 }
 free(yolo->names);
 free(yolo);
 yolo_object **ptr_yolo=&yolo;
 (*ptr_yolo)=NULL;
}

yolo_status yolo_init(yolo_object **yolo_obj, char *workingDir, char *datacfg, char *cfgfile, char *weightfile)
{
 clock_t time=clock();

 if((*yolo_obj) == NULL)
 {
  yolo_cleanup(*yolo_obj);
 }

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
 char *bad_ptr=NULL;
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

yolo_status parse_detections(yolo_object *yolo, detection *dets, yolo_detection **yolo_detect, int nboxes, int classes, float thresh)
{
 struct map_t *map=map_create();
 if(map == NULL)
 {
  return yolo_cannot_alloc_map;
 }
 int class_index;
 detection *det;

 for(int i=0; i<nboxes; ++i)
 {
  class_index=-1;
  det=NULL;
  for(int j=0; j<classes; ++j)
  {
   if(dets[i].prob[j]>=thresh)
   {
    if(det == NULL || (dets[i].prob[j]>det->prob[class_index]))
    {
     class_index=j;
     det=dets+i;
    }
   }
  }
  if(class_index>-1 && det != NULL)
  {
   map_set(map, class_index, det);
  }
 }

 (*yolo_detect)=calloc(1, sizeof(yolo_detection));
 if((*yolo_detect) == NULL)
 {
  return yolo_cannot_alloc_yolo_detection;
 }
 memset((*yolo_detect), 0, sizeof(yolo_detection));
 yolo_detection *yolo_dets=(*yolo_detect);

 if(map_empty(map))
 {
  map_free(map);
  return yolo_ok;
 }
 yolo_dets->num_boxes=map_size(map);

 yolo_dets->detection=calloc((size_t)yolo_dets->num_boxes, sizeof(detect));
 if(yolo_dets->detection == NULL)
 {
  return yolo_cannot_alloc_detect;
 }

 int i=0;
 size_t strlength;
 for(struct map_t *m=map; m != NULL; m=m->nxt)
 {
  strlength=strlen(yolo->names[m->key]);
  yolo->names[m->key][strlength]='\0';
  yolo_dets->detection[i].class_name=calloc(strlength+1, sizeof(char));
  strcpy(yolo_dets->detection[i].class_name, yolo->names[m->key]);
  yolo_dets->detection[i].probability=m->value->prob[m->key]*100;
  box *bbox;
  yolo_dets->detection[i].bbox=m->value->bbox;
  bbox=&yolo_dets->detection[i].bbox;
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
  i++;
 }
 map_free(map);
 return yolo_ok;
}

yolo_status yolo_detect(yolo_object *yolo, yolo_detection **detect, char *filename, float thresh)
{
 if(yolo == NULL)
 {
  return yolo_object_is_not_initialized;
 }

 layer l=yolo->net->layers[yolo->net->n-1];
 clock_t time;
 float nms=0.45;

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
 image im=load_image_color(filename, 0, 0);
 image sized=resize_image(im, yolo->net->w, yolo->net->h);
 float *X=sized.data;
 time=clock();
 network_predict(yolo->net, X);
 printf("%s: Predicted in %f seconds.\n", filename, sec(clock()-time));

 int nboxes=0;
 detection *dets=get_network_boxes(yolo->net, im.w, im.h, thresh, 0.5, 0, 0, &nboxes);
 if(nms)
 {
  do_nms_sort(dets, l.side*l.side*l.n, l.classes, nms);
 }

 yolo_status status=parse_detections(yolo, dets, detect, nboxes, l.classes, thresh);
 if(status != yolo_ok)
 {
  return status;
 }

 free_detections(dets, nboxes);
 free_image(im);
 free_image(sized);

 return yolo_ok;
}

void yolo_detection_free(yolo_detection **yolo)
{
 yolo_detection *yolo_det=*yolo;
 for(int i=0; i<yolo_det->num_boxes; i++)
 {
  free(yolo_det->detection[i].class_name);
 }
 free(yolo_det->detection);
 free(yolo_det);
 (*yolo)=NULL;
}

yolo_status_detailed yolo_status_decode(yolo_status status)
{
 yolo_status_detailed status_detailed;
 status_detailed.error_code=status;
 switch(status)
 {
  case yolo_cannot_alloc_detect:
   status_detailed.error_message="Cannot allocate detect in memory";
   break;
  case yolo_cannot_alloc_yolo_detection:
   status_detailed.error_message="Cannot allocate yolo_detection in memory";
   break;
  case yolo_cannot_alloc_node_yolo_object:
   status_detailed.error_message="Cannot allocate node_yolo_object in memory";
   break;
  case yolo_cannot_alloc_map:
   status_detailed.error_message="Cannot allocate map in memory";
   break;
  case yolo_cannot_change_to_working_dir:
   status_detailed.error_message="Cannot change to working directory";
   break;
  case yolo_object_is_not_initialized:
   status_detailed.error_message="yolo_object isn't allocated in memory";
   break;
  case yolo_working_dir_is_not_exists:
   status_detailed.error_message="working directory don't exists";
   break;
  case yolo_datacfg_is_not_exists:
   status_detailed.error_message="datacfg don't exists";
   break;
  case yolo_cfgfile_is_not_exists:
   status_detailed.error_message="cfgfile don't exists";
   break;
  case yolo_weight_file_is_not_exists:
   status_detailed.error_message="weight file don't exists";
   break;
  case yolo_working_dir_is_not_readable:
   status_detailed.error_message="working directory isn't readable";
   break;
  case yolo_datacfg_is_not_readable:
   status_detailed.error_message="datacfg isn't readable";
   break;
  case yolo_cfgfile_is_not_readable:
   status_detailed.error_message="cfgfile isn't readable";
   break;
  case yolo_weight_file_is_not_readable:
   status_detailed.error_message="weight file isn't readable";
   break;
  case yolo_names_file_is_not_exists:
   status_detailed.error_message="names file don't exists";
   break;
  case yolo_names_file_is_not_readable:
   status_detailed.error_message="names file isn't readable";
   break;
  default:
   status_detailed.error_code=-1;
   status_detailed.error_message="Unknow error";
 }
 return status_detailed;
}