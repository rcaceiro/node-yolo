#include "module.h"
#include "napi_yolo_errors.h"
#include <cassert>

napi_status get_string_value(napi_env env, napi_value args[], size_t index, char **value, size_t value_size)
{
 napi_status status;
 napi_valuetype value_type;
 size_t length=0;
 status=napi_typeof(env, args[index], &value_type);
 if(status != napi_ok)
 {
  return napi_invalid_arg;
 }

 if(value_type != napi_string)
 {
  return napi_string_expected;
 }

 status=napi_get_value_string_utf8(env, args[index], *value, value_size, &length);
 assert(status == napi_ok);
 ++length;
 if(length>value_size)
 {
  (*value)=static_cast<char *>(realloc(*value, length));
  assert((*value) != nullptr);
 }
 status=napi_get_value_string_utf8(env, args[index], *value, length, &length);
 assert(status == napi_ok);

 return napi_ok;
}

napi_status get_double_value(napi_env env, napi_value args[], size_t index, double *value)
{
 napi_status status;
 napi_valuetype value_type;
 status=napi_typeof(env, args[index], &value_type);
 if(status != napi_ok)
 {
  return napi_invalid_arg;
 }

 if(value_type != napi_number)
 {
  return napi_number_expected;
 }

 status=napi_get_value_double(env, args[index], value);
 assert(status == napi_ok);
 return napi_ok;
}

napi_status get_int_value(napi_env env, napi_value args[], size_t index, int *value)
{
 napi_status status;
 napi_valuetype value_type;
 status=napi_typeof(env, args[index], &value_type);
 if(status != napi_ok)
 {
  return napi_invalid_arg;
 }

 if(value_type != napi_number)
 {
  return napi_number_expected;
 }

 status=napi_get_value_int32(env, args[index], value);
 assert(status == napi_ok);
 return napi_ok;
}

yolo_napi_status load_box_object(napi_env env, box img_box, napi_value jsbox)
{
 napi_status status;
 napi_value x, y, w, h;

 status=napi_create_double(env, img_box.x, &x);
 if(status != napi_ok)
 {
  return yolo_napi_create_box_x_double_failed;
 }
 status=napi_set_named_property(env, jsbox, "x", x);
 if(status != napi_ok)
 {
  return yolo_napi_create_box_x_named_property_failed;
 }

 status=napi_create_double(env, img_box.y, &y);
 if(status != napi_ok)
 {
  return yolo_napi_create_box_y_double_failed;
 }
 status=napi_set_named_property(env, jsbox, "y", y);
 if(status != napi_ok)
 {
  return yolo_napi_create_box_y_named_property_failed;
 }

 status=napi_create_double(env, img_box.w, &w);
 if(status != napi_ok)
 {
  return yolo_napi_create_box_w_double_failed;
 }
 status=napi_set_named_property(env, jsbox, "w", w);
 if(status != napi_ok)
 {
  return yolo_napi_create_box_w_named_property_failed;
 }

 status=napi_create_double(env, img_box.h, &h);
 if(status != napi_ok)
 {
  return yolo_napi_create_box_h_double_failed;
 }
 status=napi_set_named_property(env, jsbox, "h", h);
 if(status != napi_ok)
 {
  return yolo_napi_create_box_h_named_property_failed;
 }
 return yolo_napi_ok;
}

yolo_napi_status load_detections(napi_env env, yolo_detection_image *img_detections, napi_value jsarray)
{
 napi_status status;
 napi_value jsobj, box_object, classes, prob;
 detect det;
 for(size_t i=0; i<img_detections->num_boxes; ++i)
 {
  det=img_detections->detection[i];
  status=napi_create_object(env, &jsobj);
  if(status != napi_ok)
  {
   return yolo_napi_create_object_failed;
  }

  status=napi_create_string_utf8(env, det.class_name, strlen(det.class_name), &classes);
  if(status != napi_ok)
  {
   return yolo_napi_create_class_name_string_failed;
  }
  status=napi_set_named_property(env, jsobj, "className", classes);
  if(status != napi_ok)
  {
   return yolo_napi_set_class_name_property_failed;
  }

  status=napi_create_double(env, det.probability, &prob);
  if(status != napi_ok)
  {
   return yolo_napi_create_probability_double_failed;
  }
  status=napi_set_named_property(env, jsobj, "probability", prob);
  if(status != napi_ok)
  {
   return yolo_napi_set_probability_property_failed;
  }

  status=napi_create_object(env, &box_object);
  if(status != napi_ok)
  {
   return yolo_napi_create_box_object_failed;
  }
  status=napi_set_named_property(env, jsobj, "box", box_object);
  if(status != napi_ok)
  {
   return yolo_napi_set_box_property_failed;
  }

  if(load_box_object(env, det.bbox, box_object) == yolo_napi_ok)
  {
   status=napi_set_element(env, jsarray, (uint32_t)i, jsobj);
   if(status != napi_ok)
   {
    return yolo_napi_set_object_to_array_failed;
   }
  }
 }
 return yolo_napi_ok;
}

yolo_napi_status load_detection_object(napi_env env, yolo_detection_image *img_detections, napi_value object)
{
 napi_value js_detections_array;
 napi_value js_time_spent_for_classification;
 yolo_napi_status yolo_stats;

 if(napi_create_double(env, img_detections->time_spent_for_classification, &js_time_spent_for_classification) != napi_ok)
 {
  return yolo_napi_create_object_time_spent_for_classification_double_failed;
 }

 if(napi_set_named_property(env, object, "timeSpentForClassification", js_time_spent_for_classification) != napi_ok)
 {
  return yolo_napi_create_object_time_spent_for_classification_named_property_failed;
 }

 if(napi_create_array_with_length(env, static_cast<size_t>(img_detections->num_boxes), &js_detections_array) != napi_ok)
 {
  return yolo_napi_create_array_failed;
 }

 yolo_stats=load_detections(env, img_detections, js_detections_array);

 if(yolo_stats != yolo_napi_ok)
 {
  return yolo_stats;
 }

 if(napi_set_named_property(env, object, "detections", js_detections_array) != napi_ok)
 {
  return yolo_napi_create_object_time_spent_for_classification_named_property_failed;
 }

 return yolo_napi_ok;
}

yolo_napi_status load_video_detection_object(napi_env env, yolo_detection_video *video_detections, napi_value frames_array)
{
 yolo_napi_status yolo_stats;

 for(size_t i=0; i<video_detections->count; ++i)
 {
  napi_value object;
  napi_value frame_id;
  napi_value milisecond;

  if(napi_create_object(env, &object) != napi_ok)
  {
   return yolo_napi_create_main_object_failed;
  }

  if(napi_create_int64(env, video_detections->frame_detections[i].frame, &frame_id) != napi_ok)
  {
   return yolo_napi_create_frame_failed;
  }

  if(napi_create_double(env, video_detections->frame_detections[i].milisecond, &milisecond) != napi_ok)
  {
   return yolo_napi_create_second_failed;
  }

  if(napi_set_named_property(env, object, "frame_number", frame_id) != napi_ok)
  {
   return yolo_napi_set_frame_to_object_failed;
  }

  if(napi_set_named_property(env, object, "milisecond", milisecond) != napi_ok)
  {
   return yolo_napi_set_second_to_object_failed;
  }

  yolo_stats=load_detection_object(env, &video_detections->frame_detections[i].detection_frame, object);
  if(yolo_stats != yolo_napi_ok)
  {
   return yolo_napi_create_main_object_failed;
  }

  if(napi_set_element(env, frames_array, static_cast<uint32_t>(i), object) != napi_ok)
  {
   return yolo_napi_set_array_property_failed;
  }
 }

 return yolo_napi_ok;
}

void reject(napi_env env, yolo_napi_status yolo_stats, data_holder *holder)
{
 napi_value error;
 yolo_napi_status_detailed status_detailed=yolo_napi_status_decode(yolo_stats, &holder->yolo_stats);
 if(napi_create_object(env, &error) == napi_ok)
 {
  napi_value string_error;
  if(napi_create_string_utf8(env, status_detailed.error_message, strlen(status_detailed.error_message), &string_error) == napi_ok)
  {
   napi_set_named_property(env, error, "errorMessage", string_error);
  }
  napi_value error_code;
  if(napi_create_int32(env, status_detailed.error_code, &error_code) == napi_ok)
  {
   napi_set_named_property(env, error, "errorCode", error_code);
  }
  napi_value error_prefix;
  if(napi_create_string_utf8(env, status_detailed.error_prefix, strlen(status_detailed.error_message), &error_prefix) == napi_ok)
  {
   napi_set_named_property(env, error, "errorPrefix", error_prefix);
  }
 }
 napi_reject_deferred(env, holder->deferred, error);
}

void release_async_work(napi_env env, data_holder *holder)
{
 napi_async_work work=holder->work;

 free(holder->image_path);
 free(holder);

 napi_delete_async_work(env, work);
}

void complete_async_detect(napi_env env, napi_status status, void *data)
{
 auto *holder=static_cast<data_holder *>(data);
 napi_value return_value;
 yolo_napi_status yolo_stats;

 if(holder->yolo_stats != yolo_ok)
 {
  reject(env, yolo_napi_error_from_libyolo, holder);
  release_async_work(env, holder);
  return;
 }

 if(status != napi_ok)
 {
  reject(env, yolo_napi_unknow_error, holder);
  release_async_work(env, holder);
  return;
 }

 if(holder->img_detection == nullptr && holder->video_detection == nullptr)
 {
  reject(env, yolo_napi_unknow_error, holder);
  release_async_work(env, holder);
  return;
 }

 if(holder->img_detection != nullptr)
 {
  if(napi_create_object(env, &return_value) != napi_ok)
  {
   reject(env, yolo_napi_create_main_object_failed, holder);
  }
  else
  {
   yolo_stats=load_detection_object(env, holder->img_detection, return_value);

   if(yolo_stats == yolo_napi_ok)
   {
    napi_resolve_deferred(env, holder->deferred, return_value);
   }
   else
   {
    reject(env, yolo_stats, holder);
   }
  }
  yolo_detection_image_free(&holder->img_detection);
 }
 else
 {
  if(napi_create_array_with_length(env, holder->video_detection->count, &return_value) != napi_ok)
  {
   reject(env, yolo_napi_create_main_object_failed, holder);
  }
  else
  {
   yolo_stats=load_video_detection_object(env, holder->video_detection, return_value);

   if(yolo_stats == yolo_napi_ok)
   {
    napi_resolve_deferred(env, holder->deferred, return_value);
   }
   else
   {
    reject(env, yolo_stats, holder);
   }
  }
  yolo_detection_video_free(&holder->video_detection);
 }

 release_async_work(env, holder);
}

void async_detect_image(napi_env env, void *data)
{
 (void)env;
 auto *holder=static_cast<data_holder *>(data);
 if(holder->yolo->created)
 {
  holder->yolo->mutex_lock();
  holder->yolo_stats=yolo_detect_image(holder->yolo->yolo, &holder->img_detection, holder->image_path, holder->thresh_value);
  holder->yolo->mutex_unlock();
 }
 else
 {
  holder->yolo_stats=yolo_instanciation;
 }
}

void async_detect_video(napi_env env, void *data)
{
 (void)env;
 auto *holder=static_cast<data_holder *>(data);
 if(holder->yolo->created)
 {
  holder->yolo->mutex_lock();
  holder->yolo_stats=yolo_detect_video(holder->yolo->yolo, &holder->video_detection, holder->image_path, holder->thresh_value, holder->fraction_frames_to_process);
  holder->yolo->mutex_unlock();
 }
 else
 {
  holder->yolo_stats=yolo_instanciation;
 }
}

napi_ref Yolo::constructor;

Yolo::Yolo(char *working_directory, char *datacfg, char *cfgfile, char *weightfile) : env_(nullptr), wrapper_(nullptr)
{
 this->yolo=nullptr;
 yolo_status yolo_stats=yolo_init(&this->yolo, working_directory, datacfg, cfgfile, weightfile);
 if(yolo_stats != yolo_ok)
 {
  this->created=false;
  return;
 }
 pthread_mutex_init(&this->mutex, nullptr);
 this->created=true;
}

Yolo::~Yolo()
{
 pthread_mutex_destroy(&this->mutex);
 yolo_cleanup(this->yolo);
 napi_delete_reference(env_, wrapper_);
}

void Yolo::Destructor(napi_env env, void *nativeObject, void * /*finalize_hint*/)
{
 (void)env;
 reinterpret_cast<Yolo *>(nativeObject)->~Yolo();
}

napi_value Yolo::Init(napi_env env, napi_value exports)
{
 napi_status status;
 napi_property_descriptor properties[]={{"detectImage\0", nullptr, Yolo::DetectImage, nullptr, nullptr, nullptr, napi_default, nullptr},
                                        {"detectVideo\0", nullptr, Yolo::DetectVideo, nullptr, nullptr, nullptr, napi_default, nullptr}};

 napi_value cons;
 status=napi_define_class(env, "Yolo\0", NAPI_AUTO_LENGTH, Yolo::New, nullptr, 2, properties, &cons);
 assert(status == napi_ok);

 status=napi_create_reference(env, cons, 1, &Yolo::constructor);
 assert(status == napi_ok);

 status=napi_set_named_property(env, exports, "Yolo", cons);
 assert(status == napi_ok);
 return exports;
}

napi_value Yolo::New(napi_env env, napi_callback_info info)
{
 napi_status status;
 napi_value target;
 status=napi_get_new_target(env, info, &target);
 assert(status == napi_ok);
 bool is_constructor=target != nullptr;

 if(is_constructor)
 {
  // Invoked as constructor: `new MyObject(...)`
  size_t argc=4;
  napi_value args[4];
  napi_value jsthis;
  status=napi_get_cb_info(env, info, &argc, args, &jsthis, nullptr);
  assert(status == napi_ok);
  assert(argc>=4);
  char *darknet_path=nullptr;
  char *datacfg=nullptr;
  char *cfgfile=nullptr;
  char *weightfile=nullptr;

  get_string_value(env, args, 0, &darknet_path, 0);
  get_string_value(env, args, 1, &datacfg, 0);
  get_string_value(env, args, 2, &cfgfile, 0);
  get_string_value(env, args, 3, &weightfile, 0);

  auto obj=new Yolo(darknet_path, datacfg, cfgfile, weightfile);
  obj->env_=env;
  status=napi_wrap(env, jsthis, reinterpret_cast<void *>(obj), Yolo::Destructor, nullptr, &obj->wrapper_);
  assert(status == napi_ok);

  return jsthis;
 }
 else
 {
  // Invoked as plain function `MyObject(...)`, turn into construct call.
  size_t argc_=4;
  napi_value args[4];
  status=napi_get_cb_info(env, info, &argc_, args, nullptr, nullptr);
  assert(status == napi_ok);

  const size_t argc=4;
  napi_value argv[4]={args[0], args[1], args[2], args[3]};

  napi_value cons;
  status=napi_get_reference_value(env, Yolo::constructor, &cons);
  assert(status == napi_ok);

  napi_value instance;
  status=napi_new_instance(env, cons, argc, argv, &instance);
  assert(status == napi_ok);

  return instance;
 }
}

napi_value Yolo::DetectImage(napi_env env, napi_callback_info info)
{
 napi_status status;
 napi_deferred deferred;
 napi_value promise;
 napi_value jsthis;
 size_t argc=2;
 napi_value args[2];

 status=napi_get_cb_info(env, info, &argc, args, &jsthis, nullptr);
 if(status != napi_ok)
 {
  napi_throw_error(env, "00", "Cannot get arguments");
  return nullptr;
 }

 if(argc<2)
 {
  napi_throw_error(env, "01", "You have to pass the path to image and thresh value as parameters");
  return nullptr;
 }

 char *image_path=nullptr;
 status=get_string_value(env, args, 0, &image_path, 0);
 assert(status == napi_ok);
 if(image_path == nullptr)
 {
  napi_throw_error(env, "02", "Cannot get image path");
  return nullptr;
 }

 double thresh=0.5;
 status=get_double_value(env, args, 1, &thresh);
 if(status != napi_ok)
 {
  napi_throw_error(env, "04", "Cannot get thresh value");
  return nullptr;
 }

 void *obj=nullptr;
 status=napi_unwrap(env, jsthis, &obj);
 assert(status == napi_ok);
 auto *yolo_obj=static_cast<Yolo *>(obj);

 napi_value resource_name;
 napi_value resource;
 status=napi_create_string_utf8(env, "nodeyolo.detect_image", 18, &resource_name);
 assert(status == napi_ok);
 status=napi_create_object(env, &resource);
 assert(status == napi_ok);

 auto *holder=static_cast<data_holder *>(calloc(1, sizeof(data_holder)));
 if(holder == nullptr)
 {
  napi_throw_error(env, "03", "Cannot allocate a struct in memory");
  return nullptr;
 }

 status=napi_create_promise(env, &deferred, &promise);
 assert(status == napi_ok);

 holder->deferred=deferred;
 holder->image_path=image_path;
 holder->thresh_value=(float)thresh;
 holder->yolo=yolo_obj;
 holder->resource=resource;

 status=napi_create_async_work(env, resource, resource_name, async_detect_image, complete_async_detect, holder, &holder->work);
 assert(status == napi_ok);
 status=napi_queue_async_work(env, holder->work);
 assert(status == napi_ok);

 return promise;
}

napi_value Yolo::DetectVideo(napi_env env, napi_callback_info info)
{
 napi_status status;
 napi_deferred deferred;
 napi_value promise;
 napi_value jsthis;
 size_t argc=3;
 napi_value args[3];

 status=napi_get_cb_info(env, info, &argc, args, &jsthis, nullptr);
 if(status != napi_ok)
 {
  napi_throw_error(env, "00", "Cannot get arguments");
  return nullptr;
 }

 if(argc<1)
 {
  napi_throw_error(env, "01", "You have to pass the path to image as parameters");
  return nullptr;
 }

 char *video_path=nullptr;
 status=get_string_value(env, args, 0, &video_path, 0);
 if(status != napi_ok || video_path == nullptr)
 {
  napi_throw_error(env, "02", "Cannot get video path");
  return nullptr;
 }

 double thresh=0.5;
 if(argc>1)
 {
  status=get_double_value(env, args, 1, &thresh);
  if(status != napi_ok)
  {
   napi_throw_error(env, "04", "Cannot get thresh value");
   return nullptr;
  }
 }

 double fraction_frames_to_process=1;
 if(argc>2)
 {
  status=get_double_value(env, args, 2, &fraction_frames_to_process);
  if(status != napi_ok)
  {
   napi_throw_error(env, "05", "Cannot get fraction to process frames value");
   return nullptr;
  }
 }

 void *obj=nullptr;
 status=napi_unwrap(env, jsthis, &obj);
 assert(status == napi_ok);
 auto *yolo_obj=static_cast<Yolo *>(obj);

 napi_value resource_name;
 napi_value resource;
 status=napi_create_string_utf8(env, "nodeyolo.detect_video", 18, &resource_name);
 assert(status == napi_ok);
 status=napi_create_object(env, &resource);
 assert(status == napi_ok);

 auto *holder=static_cast<data_holder *>(calloc(1, sizeof(data_holder)));
 if(holder == nullptr)
 {
  napi_throw_error(env, "03", "Cannot allocate a struct in memory");
  return nullptr;
 }

 status=napi_create_promise(env, &deferred, &promise);
 if(status != napi_ok)
 {
  napi_throw_error(env, "06", "Cannot create promise");
  return nullptr;
 }

 holder->deferred=deferred;
 holder->image_path=video_path;
 holder->thresh_value=(float)thresh;
 holder->yolo=yolo_obj;
 holder->resource=resource;
 holder->fraction_frames_to_process=fraction_frames_to_process;

 status=napi_create_async_work(env, resource, resource_name, async_detect_video, complete_async_detect, holder, &holder->work);
 assert(status == napi_ok);
 status=napi_queue_async_work(env, holder->work);
 assert(status == napi_ok);

 return promise;
}

void Yolo::mutex_lock()
{
 pthread_mutex_lock(&this->mutex);
}

void Yolo::mutex_unlock()
{
 pthread_mutex_unlock(&this->mutex);
}

NAPI_MODULE(NodeYolo, Yolo::Init);