#include <darknet/src/cuda.h>
#include "libyolo.h"
#include "map_lib.h"

void yolo_cleanup(yolo_object *yolo)
{
 free_network(yolo->net);
 free(yolo->names);
 free(yolo);
 yolo_object **ptr_yolo=&yolo;
 (*ptr_yolo)=NULL;
}

yolo_object *yolo_init(char *workingDir, char *datacfg, char *cfgfile, char *weightfile)
{
 clock_t time=clock();

 yolo_object *yolo=(yolo_object *)malloc(sizeof(yolo_object));
 if(!yolo)
 {
  return NULL;
 }
 memset(yolo, 0, sizeof(yolo_object));

 char cur_dir[1024];
 getcwd(cur_dir, sizeof(cur_dir));
 if(chdir(workingDir) == -1)
 {
  fprintf(stderr, "%s\n", strerror(errno));
  return NULL;
 }

 yolo->net=load_network(cfgfile, weightfile, 0);
 list *options=read_data_cfg(datacfg);
 char *name_list=option_find_str(options, "names", "data/names.list");
 yolo->names=get_labels(name_list);

 set_batch_network(yolo->net, 1);
 srand(2222222);

 printf("Network configured and loaded in %f seconds\n", sec(clock()-time));
 chdir(cur_dir);
 return yolo;
}

yolo_detection *parse_detections(yolo_object *yolo, detection *dets, int nboxes, int classes, float thresh, image im)
{
 struct map_t *map=map_create();
 if(map == NULL)
 {
  fprintf(stderr, "Cannot allocate map in memory");
  return NULL;
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

 if(map_empty(map))
 {
  return NULL;
 }

 yolo_detection *yolo_dets=calloc(1, sizeof(yolo_detection));
 if(yolo_dets == NULL)
 {
  return NULL;
 }
 yolo_dets->num_boxes=map_size(map);
 yolo_dets->detection=calloc((size_t)yolo_dets->num_boxes, sizeof(detect));
 if(yolo_dets->detection == NULL)
 {
  return NULL;
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
 return yolo_dets;
}

yolo_detection *yolo_detect(yolo_object *yolo, char *filename, float thresh)
{
 if(yolo == NULL)
 {
  return NULL;
 }

 layer l=yolo->net->layers[yolo->net->n-1];
 clock_t time;
 float nms=0.45;

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

 yolo_detection *yolo_detects=parse_detections(yolo, dets, nboxes, l.classes, 0.5, im);

 free_detections(dets, nboxes);
 free_image(im);
 free_image(sized);
#ifdef OPENCV
 cvWaitKey(0);
       cvDestroyAllWindows();
#endif

 return yolo_detects;
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